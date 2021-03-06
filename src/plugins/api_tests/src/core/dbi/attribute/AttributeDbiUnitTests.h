#ifndef _U2_ATTRIBUTE_DBI_UNIT_TEST_H_
#define _U2_ATTRIBUTE_DBI_UNIT_TEST_H_

#include "core/dbi/DbiTest.h"

#include <U2Core/U2AttributeDbi.h>
#include <U2Core/U2DbiRegistry.h>
#include <U2Test/TestRunnerSettings.h>
#include <unittest.h>

namespace U2 {

class AttributeTestData {
public:
    static U2AttributeDbi* getAttributeDbi();
    static QList<U2DataId>* getObjects() { return objects; }
    static void init();
    static void shutdown();
    static void testAttributesMatch(QList<U2IntegerAttribute>& expectedInt,
                         QList<U2RealAttribute>& expectedReal,
                         QList<U2StringAttribute>& expectedString,
                         QList<U2ByteArrayAttribute>& expectedArray,
                         const QList<U2DataId>& attributes);
    static void getObjectAttrsTest(const APITestData& testData,
                        bool filterByName,
                        bool filterByChild);
protected:

    static U2AttributeDbi* attributeDbi;
    static QList<U2DataId>* objects;

    static const QString& ATT_DB_URL;

    static TestDbiProvider dbiProvider;
    static bool registerTest;
};

class ObjectAttributesTestData {
public:
    U2DataId objId;
    QList<U2IntegerAttribute> intAttrs;
    QList<U2RealAttribute> realAttrs;
    QList<U2StringAttribute> stringAttrs;
    QList<U2ByteArrayAttribute> byteArrAttrs;
};

class AttributeDbiUnitTests_getAvailableAttributeNames : public UnitTest {
public:
    void Test();
};

class AttributeDbiUnitTests_getObjectAttributes : public UnitTest {
public:
    void Test();
};

class AttributeDbiUnitTests_getObjectAttributesByName : public UnitTest {
public:
    void Test();
};

class AttributeDbiUnitTests_getObjectPairAttributes : public UnitTest {
public:
    void Test();
};

class AttributeDbiUnitTests_getObjectPairAttributesByName : public UnitTest {
public:
    void Test();
};

class AttributeDbiUnitTests_removeAttributes : public UnitTest {
public:
    void Test();
};

class AttributeDbiUnitTests_removeObjectAttributes : public UnitTest {
public:
    void Test();
};

class AttributeDbiUnitTests_IntegerAttribute : public UnitTest {
public:
    void Test();
};

class AttributeDbiUnitTests_RealAttribute : public UnitTest {
public:
    void Test();
};


class AttributeDbiUnitTests_StringAttribute : public UnitTest {
public:
    void Test();
};

class AttributeDbiUnitTests_ByteArrayAttribute : public UnitTest {
public:
    void Test();
};

} //namespace

Q_DECLARE_METATYPE(U2::U2IntegerAttribute);
Q_DECLARE_METATYPE(U2::U2RealAttribute);
Q_DECLARE_METATYPE(U2::U2StringAttribute);
Q_DECLARE_METATYPE(U2::U2ByteArrayAttribute);
Q_DECLARE_METATYPE(U2::ObjectAttributesTestData);

Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_ByteArrayAttribute);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_getAvailableAttributeNames);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_getObjectAttributes);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_getObjectAttributesByName);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_getObjectPairAttributes);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_getObjectPairAttributesByName);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_IntegerAttribute);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_RealAttribute);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_removeAttributes);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_removeObjectAttributes);
Q_DECLARE_METATYPE(U2::AttributeDbiUnitTests_StringAttribute);

#endif
