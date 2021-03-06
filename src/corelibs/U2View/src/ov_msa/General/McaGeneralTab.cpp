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

#include "McaGeneralTab.h"

#include "../McaEditor.h"
#include <U2Gui/ShowHideSubgroupWidget.h>

namespace U2 {

McaGeneralTab::McaGeneralTab(McaEditor *mca)
    : mca(mca) {
    setupUi(this);
    consensusModeWgt->init(mca->getMaObject(), mca->getUI()->getConsensusArea());

    ShowHideSubgroupWidget* alignmentInfo = new ShowHideSubgroupWidget("ALIGNMENT_INFO", tr("Alignment info"),
                                                                       alignmentInfoWgt, true);
    ShowHideSubgroupWidget* consensusMode = new ShowHideSubgroupWidget("CONSENSUS_MODE", tr("Consensus mode"),
                                                                       consensusModeWgt, true);

    Ui_McaGeneralTab::verticalLayout->addWidget(alignmentInfo);
    Ui_McaGeneralTab::verticalLayout->addWidget(consensusMode);

    lengthLabel->setText(QString::number(mca->getAlignmentLen()));
    seqNumLabel->setText(QString::number(mca->getNumSequences()));

    connect(mca->getMaObject(),
            SIGNAL(si_alignmentChanged(MultipleAlignment, MaModificationInfo)),
            SLOT(sl_alignmentChanged()));
}

void McaGeneralTab::sl_alignmentChanged() {
    lengthLabel->setText(QString::number(mca->getAlignmentLen()));
    seqNumLabel->setText(QString::number(mca->getNumSequences()));
}

} // namespace
