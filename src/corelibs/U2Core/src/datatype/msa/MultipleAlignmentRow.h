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

#ifndef _U2_MULTIPLE_ALIGNMENT_ROW_H_
#define _U2_MULTIPLE_ALIGNMENT_ROW_H_

#include <QSharedPointer>

#include <U2Core/MsaRowUtils.h>
#include <U2Core/U2Msa.h>
#include <U2Core/U2OpStatus.h>

namespace U2 {

class MultipleAlignment;
class MultipleAlignmentData;
class MultipleAlignmentRowData;
class U2OpStatus;

class U2CORE_EXPORT MultipleAlignmentRow {
    friend class MultipleAlignmentData;

protected:
    MultipleAlignmentRow(MultipleAlignmentRowData *maRowData);

public:
    virtual ~MultipleAlignmentRow();

    MultipleAlignmentRowData * data() const;
    template <class Derived> inline Derived dynamicCast() const;
    template <class Derived> inline Derived dynamicCast(U2OpStatus &os) const;

    MultipleAlignmentRowData & operator*();
    const MultipleAlignmentRowData & operator*() const;

    MultipleAlignmentRowData * operator->();
    const MultipleAlignmentRowData * operator->() const;

protected:
    QSharedPointer<MultipleAlignmentRowData> maRowData;
};

template <class Derived>
Derived MultipleAlignmentRow::dynamicCast() const {
    return Derived(*this);
}

template <class Derived>
Derived MultipleAlignmentRow::dynamicCast(U2OpStatus &os) const {
    Derived derived(*this);
    if (NULL == derived.maRowData) {
        assert(false);
        os.setError("Can't cast MultipleAlignmentRow to a derived class");
    }
    return derived;
}

/**
 * A row in a multiple alignment structure.
 * The row consists of a sequence without gaps
 * and a gap model.
 * A row core is an obsolete concept. Currently,
 * it exactly equals to the row (offset always equals to zero).
 */
class U2CORE_EXPORT MultipleAlignmentRowData {
public:
    virtual ~MultipleAlignmentRowData();

    /** Name of the row, can be empty */
    virtual QString getName() const = 0;
    virtual void setName(const QString &name) = 0;

    /** Returns ID of the row in the database. */
    virtual qint64 getRowId() const = 0;
    virtual void setRowId(qint64 rowId) = 0;

    virtual char charAt(qint64 position) const = 0;

    virtual QByteArray toByteArray(U2OpStatus &os, qint64 length) const = 0;

    virtual qint64 getRowLengthWithoutTrailing() const = 0;
    virtual qint64 getCoreStart() const = 0;
    virtual qint64 getCoreLength() const = 0;
    virtual qint64 getBaseCount(qint64 beforePosition) const = 0;

    virtual void crop(U2OpStatus &os, qint64 startPosition, qint64 count) = 0;

    virtual bool operator !=(const MultipleAlignmentRowData &other) const = 0;
    virtual bool operator ==(const MultipleAlignmentRowData &other) const = 0;
};

inline bool	operator!=(const MultipleAlignmentRow &ptr1, const MultipleAlignmentRow &ptr2) { return *ptr1 != *ptr2; }
inline bool	operator!=(const MultipleAlignmentRow &ptr1, const MultipleAlignmentRowData *ptr2) { return *ptr1 != *ptr2; }
inline bool	operator!=(const MultipleAlignmentRowData *ptr1, const MultipleAlignmentRow &ptr2) { return *ptr1 != *ptr2; }
inline bool	operator==(const MultipleAlignmentRow &ptr1, const MultipleAlignmentRow &ptr2) { return *ptr1 == *ptr2; }
inline bool	operator==(const MultipleAlignmentRow &ptr1, const MultipleAlignmentRowData *ptr2) { return *ptr1 == *ptr2; }
inline bool	operator==(const MultipleAlignmentRowData *ptr1, const MultipleAlignmentRow &ptr2) { return *ptr1 == *ptr2; }

}   // namespace U2

#endif // _U2_MULTIPLE_ALIGNMENT_ROW_H_
