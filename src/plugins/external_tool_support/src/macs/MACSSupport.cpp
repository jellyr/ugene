/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2013 UniPro <ugene@unipro.ru>
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

#include "MACSSupport.h"

#include <python/PythonSupport.h>

#include <U2Core/AppContext.h>

namespace U2 {

const QString MACSSupport::TOOL_NAME("MACS");

MACSSupport::MACSSupport(const QString &name)
: ExternalTool(name)
{
    initialize();
}

void MACSSupport::initialize() {
    if (AppContext::getMainWindow()) {
        icon = QIcon(":external_tool_support/images/cmdline.png");
        grayIcon = QIcon(":external_tool_support/images/cmdline_gray.png");
        warnIcon = QIcon(":external_tool_support/images/cmdline_warn.png");
    }

    toolKitName = "Cistrome";
    description = MACSSupport::tr("<i>MACS</i> - Model-based Analysis of ChIP-Seq data, which analyzes data"
        "generated by short read sequencers such as Solexa's Genome Analyzer. MACS empirically models the shift"
        "size of ChIP-Seq tags, and uses it to improve the spatial resolution of predicted binding sites."
        "MACS also uses a dynamic Poisson distribution to effectively capture local biases in the genome,"
        "allowing for more robust predictions. MACS compares favorably to existing ChIP-Seq peak-finding algorithms, and is freely available.");

    executableFileName = "macs14.py";

    toolRunnerProgramm = PYTHON_TOOL_NAME;
    dependencies << PYTHON_TOOL_NAME;

    validMessage = "macs14";
    validationArguments << "--version";

    versionRegExp=QRegExp(executableFileName + " (\\d+\\.\\d+\\.\\d+)");

}

} // U2
