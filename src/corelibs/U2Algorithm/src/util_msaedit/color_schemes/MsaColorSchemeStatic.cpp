/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2017 UniPro <ugene@unipro.ru>
 * http://ugene.net
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

#include "MsaColorSchemeStatic.h"

namespace U2 {

MsaColorSchemeStatic::MsaColorSchemeStatic(QObject *parent, const MsaColorSchemeFactory *factory, MultipleAlignmentObject *maObj, const QVector<QColor> &colorsPerChar)
    : MsaColorScheme(parent, factory, maObj),
      colorsPerChar(colorsPerChar)
{

}

QColor MsaColorSchemeStatic::getColor(int /*seq*/, int /*pos*/, char c) const {
    return colorsPerChar[(quint8)c];
}

MsaColorSchemeStaticFactory::MsaColorSchemeStaticFactory(QObject *parent, const QString &id, const QString &name, DNAAlphabetType alphabetType, const QVector<QColor> &colorsPerChar)
    : MsaColorSchemeFactory(parent, id, name, alphabetType),
      colorsPerChar(colorsPerChar)
{

}

MsaColorScheme * MsaColorSchemeStaticFactory::create(QObject *parent, MultipleAlignmentObject *maObj) const {
    return new MsaColorSchemeStatic(parent, this, maObj, colorsPerChar);
}

}   // namespace U2
