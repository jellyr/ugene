/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2011 UniPro <ugene@unipro.ru>
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

#include "RawDNASequenceFormat.h"

#include "DocumentFormatUtils.h"
#include "PlainTextFormat.h"

#include <U2Core/U2OpStatus.h>
#include <U2Core/IOAdapter.h>
#include <U2Core/U2SafePoints.h>
#include <U2Core/GObjectTypes.h>
#include <U2Core/DNASequenceObject.h>

#include <U2Core/TextUtils.h>

namespace U2 {

/* TRANSLATOR U2::RawDNASequenceFormat */
/* TRANSLATOR U2::IOAdapter */

RawDNASequenceFormat::RawDNASequenceFormat(QObject* p) : DocumentFormat(p, DocumentFormatFlags_W1)
{
    formatName = tr("Raw sequence");
    fileExtensions << "seq" << "txt";
    supportedObjectTypes+=GObjectTypes::SEQUENCE;
    formatDescription = tr("Raw sequence file - a whole content of the file is treated either as a single nucleotide or peptide sequence UGENE will remove all non-alphabetic chars from the result sequence");
}


static void load(IOAdapter* io, const U2DbiRef& dbiRef,  QList<GObject*>& objects, const QVariantMap& hints, U2OpStatus& os) {
    static int READ_BUFF_SIZE = 4096;

    QByteArray readBuffer(READ_BUFF_SIZE, '\0');
    char* buff  = readBuffer.data();

    int wholeSize = io->left();
    const QBitArray& ALPHAS = TextUtils::ALPHA_NUMS;

    QByteArray seq;
    if (wholeSize != -1) {
        seq.reserve(wholeSize);
    }

    //reading sequence
    QBuffer writer(&seq);
    writer.open( QIODevice::WriteOnly | QIODevice::Append );
    bool ok = true;
    int len = 0;
    while (ok && (len = io->readBlock(buff, READ_BUFF_SIZE)) > 0) {
        if (os.isCoR()) {
            break;
        }
        for (int i=0; i<len && ok; i++) {
            char c = buff[i];
            if (ALPHAS[(uchar)c]) {
                ok = writer.putChar(c);
            }
        }
        os.setProgress(io->getProgress());
    }
    writer.close();
    CHECK_OP(os, );
    CHECK_EXT(seq.size() > 0, os.setError(RawDNASequenceFormat::tr("Sequence is empty")), );
    DNASequence dnaSeq("Sequence", seq);
    DocumentFormatUtils::addSequenceObjectDeprecated(dbiRef, dnaSeq.getName(), objects, dnaSeq, hints, os);
}

Document* RawDNASequenceFormat::loadDocument(IOAdapter* io, const U2DbiRef& dbiRef, const QVariantMap& fs, U2OpStatus& os) {
    QList<GObject*> objects;
    load(io, dbiRef, objects, fs, os);
    CHECK_OP(os, NULL);
    
    Document* doc = new Document(this, io->getFactory(), io->getURL(), dbiRef, dbiRef.isValid(), objects, fs);
    return doc;
}

FormatCheckResult RawDNASequenceFormat::checkRawData(const QByteArray& rawData, const GUrl&) const {
    const char* data = rawData.constData();
    int size = rawData.size();
    if(QRegExp("[a-zA-Z\r\n]*").exactMatch(rawData)) {
        return FormatDetection_VeryHighSimilarity;
    }
    bool hasBinaryData = TextUtils::contains(TextUtils::BINARY, data, size);
    //returning 'very low chance' here just because it's impossible to have 100% detection for this format
    return hasBinaryData ? FormatDetection_NotMatched : FormatDetection_VeryLowSimilarity; 
}

void RawDNASequenceFormat::storeDocument(Document* d, IOAdapter* io, U2OpStatus& os) {
    assert(d->getObjects().size() ==1);
    GObject* obj = d->getObjects().first();
    U2SequenceObject* so = qobject_cast<U2SequenceObject*>(obj);
    assert(so!=NULL);
    PlainTextFormat::storeRawData(so->getWholeSequenceData(), os, io);
}


}//namespace
