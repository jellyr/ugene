/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <U2Core/ChromatogramUtils.h>
#include <U2Core/DatatypeSerializeUtils.h>
#include <U2Core/DbiConnection.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNAChromatogram.h>
#include <U2Core/DNASequence.h>
#include <U2Core/McaRowInnerData.h>
#include <U2Core/MsaDbiUtils.h>
#include <U2Core/MultipleChromatogramAlignment.h>
#include <U2Core/RawDataUdrSchema.h>
#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatus.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/U2SequenceDbi.h>
#include <U2Core/U2SequenceUtils.h>

#include "McaDbiUtils.h"

namespace U2 {

void McaDbiUtils::updateMca(U2OpStatus &os, const U2EntityRef &mcaRef, const MultipleChromatogramAlignment &mca) {
    // Move to the MCAImporter
    // TODO: check, if a transaction or an operation block should be started
    DbiConnection connection(mcaRef.dbiRef, os);
    CHECK_OP(os, );

    U2AttributeDbi *attributeDbi = connection.dbi->getAttributeDbi();
    SAFE_POINT_EXT(NULL != attributeDbi, os.setError("NULL Attribute Dbi"), );

    U2MsaDbi *msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_EXT(NULL != msaDbi, os.setError("NULL Msa Dbi"), );

    U2SequenceDbi *sequenceDbi = connection.dbi->getSequenceDbi();
    SAFE_POINT_EXT(NULL != sequenceDbi, os.setError("NULL Sequence Dbi"), );

    const DNAAlphabet *alphabet = mca->getAlphabet();
    SAFE_POINT_EXT(NULL != alphabet, os.setError("The alignment alphabet is NULL"), );

    //// UPDATE MCA
    U2Msa dbMca;
    dbMca.id = mcaRef.entityId;
    dbMca.visualName = mca->getName();
    dbMca.alphabet.id = alphabet->getId();
    dbMca.length = mca->getLength();

    msaDbi->updateMsaName(mcaRef.entityId, mca->getName(), os);
    CHECK_OP(os, );

    msaDbi->updateMsaAlphabet(mcaRef.entityId, alphabet->getId(), os);
    CHECK_OP(os, );

    msaDbi->updateMsaLength(mcaRef.entityId, mca->getLength(), os);
    CHECK_OP(os, );

    //// UPDATE ROWS AND SEQUENCES
    // Get rows that are currently stored in the database
    const QList<U2McaRow> currentRows = getMcaRows(os, mcaRef);
    CHECK_OP(os, );

    QList<qint64> currentRowIds;
    QList<qint64> newRowsIds = mca->getRowsIds();
    QList<qint64> eliminatedRows;
    // TODO: get the mca folder and create child objects there
    const QString dbFolder = U2ObjectDbi::ROOT_FOLDER;

    foreach (const U2McaRow &currentRow, currentRows) {
        currentRowIds << currentRow.rowId;

        // Update data for rows with the same row and child objects IDs
        if (newRowsIds.contains(currentRow.rowId)) {
            // Update sequence and row info
            const U2McaRow newRow = mca->getMcaRowByRowId(currentRow.rowId, os)->getRowDbInfo();
            CHECK_OP(os, );

            if (newRow.chromatogramId != currentRow.chromatogramId ||
                newRow.sequenceId != currentRow.sequenceId) {
                // Kill the row from the current alignment, it is incorrect. New row with this ID will be created later.
                // TODO: replace with specific utils
                MsaDbiUtils::removeRow(mcaRef, currentRow.rowId, os);
                CHECK_OP(os, );

                currentRowIds.removeOne(currentRow.rowId);
                continue;
            }

            const McaRowMemoryData rowMemoryData = mca->getMcaRowByRowId(newRow.rowId, os)->getRowMemoryData();
            CHECK_OP(os, );

            msaDbi->updateRowName(mcaRef.entityId, newRow.rowId, rowMemoryData.sequence.getName(), os);
            CHECK_OP(os, );

            msaDbi->updateRowContent(mcaRef.entityId, newRow.rowId, rowMemoryData.sequence.seq, rowMemoryData.gapModel, os);
            CHECK_OP(os, );

            ChromatogramUtils::updateChromtogramData(os, U2EntityRef(mcaRef.dbiRef, newRow.chromatogramId), rowMemoryData.chromatogram);
            CHECK_OP(os, );
        } else {
            // Remove rows that are no more present in the alignment
            eliminatedRows.append(currentRow.rowId);
        }
    }

    msaDbi->removeRows(mcaRef.entityId, eliminatedRows, os);
    CHECK_OP(os, );

    // Add rows that are stored in memory, but are not present in the database,
    // remember the rows order
    QList<qint64> rowsOrder;
    for (int i = 0, n = mca->getNumRows(); i < n; ++i) {
        const MultipleChromatogramAlignmentRow mcaRow = mca->getMcaRow(i);
        U2McaRow dbRow = mcaRow->getRowDbInfo();

        if (!dbRow.hasValidChildObjectIds() || !currentRowIds.contains(dbRow.rowId)) {
            // Import the child objects
            const U2EntityRef chromatogramRef = ChromatogramUtils::import(os, connection.dbi->getDbiRef(), dbFolder, mcaRow->getChromatogram());
            CHECK_OP(os, );

            const U2EntityRef sequenceRef = U2SequenceUtils::import(os, connection.dbi->getDbiRef(), dbFolder, mcaRow->getSequence(), dbMca.alphabet.id);
            CHECK_OP(os, );

            // Create the row
            dbRow.rowId = U2MsaRow::INVALID_ROW_ID; // set the row ID automatically
            dbRow.chromatogramId = chromatogramRef.entityId;
            dbRow.sequenceId = sequenceRef.entityId;
            dbRow.gstart = mcaRow->getCoreStart();
            dbRow.gend = mcaRow->getCoreRegion().endPos();
            dbRow.gaps = mcaRow->getGapModel();

            McaDbiUtils::addRow(os, mcaRef, -1, dbRow);
            CHECK_OP(os, );
        }
        rowsOrder << dbRow.rowId;
    }

    //// UPDATE ROWS POSITIONS
    msaDbi->setNewRowsOrder(mcaRef.entityId, rowsOrder, os);

    //// UPDATE ALIGNMENT ATTRIBUTES
    QVariantMap info = mca->getInfo();

    foreach (const QString &key, info.keys()) {
        QString value = info.value(key).toString();
        U2StringAttribute attribute(mcaRef.entityId, key, value);

        attributeDbi->createStringAttribute(attribute, os);
        CHECK_OP(os, );
    }
}

void McaDbiUtils::addRow(U2OpStatus &os, const U2EntityRef &mcaRef, qint64 posInMca, U2McaRow &row) {
    SAFE_POINT_EXT(row.hasValidChildObjectIds(), os.setError("Invalid child objects references"), );

    DbiConnection connection(mcaRef.dbiRef, os);
    CHECK_OP(os, );

    U2MsaDbi *msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_EXT(NULL != msaDbi, os.setError("NULL Msa dbi"), );

    msaDbi->addRow(mcaRef.entityId, posInMca, row, os);
    CHECK_OP(os, );
}

void McaDbiUtils::addRows(U2OpStatus &os, const U2EntityRef &mcaRef, QList<U2McaRow> &rows) {
    for (int i = 0; i < rows.size(); i++) {
        addRow(os, mcaRef, -1, rows[i]);
        CHECK_OP(os, );
    }
}

QList<U2McaRow> McaDbiUtils::getMcaRows(U2OpStatus &os, const U2EntityRef &mcaRef) {
    QList<U2McaRow> mcaRows;

    DbiConnection connection(mcaRef.dbiRef, os);
    CHECK_OP(os, mcaRows);

    U2MsaDbi *msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_EXT(NULL != msaDbi, os.setError("MSA dbi is NULL"), mcaRows);

    const QList<U2MsaRow> msaRows = msaDbi->getRows(mcaRef.entityId, os);
    CHECK_OP(os, mcaRows);

    foreach (const U2MsaRow &msaRow, msaRows) {
        U2McaRow mcaRow(msaRow);
        mcaRow.chromatogramId = ChromatogramUtils::getChromatogramIdByRelatedSequenceId(os, U2EntityRef(mcaRef.dbiRef, msaRow.sequenceId)).entityId;
        CHECK_OP(os, mcaRows);
        mcaRows << mcaRow;
    }

    return mcaRows;
}

U2McaRow McaDbiUtils::getMcaRow(U2OpStatus &os, const U2EntityRef &mcaRef, qint64 rowId) {
    DbiConnection connection(mcaRef.dbiRef, os);
    CHECK_OP(os, U2McaRow());

    U2MsaDbi *msaDbi = connection.dbi->getMsaDbi();
    SAFE_POINT_EXT(NULL != msaDbi, os.setError("MSA dbi is NULL"), U2McaRow());

    const U2MsaRow msaRow = msaDbi->getRow(mcaRef.entityId, rowId, os);
    CHECK_OP(os, U2McaRow());

    U2McaRow mcaRow(msaRow);
    mcaRow.chromatogramId = ChromatogramUtils::getChromatogramIdByRelatedSequenceId(os, U2EntityRef(mcaRef.dbiRef, msaRow.sequenceId)).entityId;
    CHECK_OP(os, mcaRow);

    return mcaRow;
}

}   // namespace U2
