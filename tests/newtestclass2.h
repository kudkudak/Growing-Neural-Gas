/*
 * File:   newtestclass2.h
 * Author: staszek
 *
 * Created on 2012-08-07, 22:10:52
 */

#ifndef NEWTESTCLASS2_H
#define	NEWTESTCLASS2_H

#include <cppunit/extensions/HelperMacros.h>

class newtestclass2 : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(newtestclass2);

    CPPUNIT_TEST(testMethod);
    CPPUNIT_TEST(testFailedMethod);

    CPPUNIT_TEST_SUITE_END();

public:
    newtestclass2();
    virtual ~newtestclass2();
    void setUp();
    void tearDown();

private:
    void testMethod();
    void testFailedMethod();
};

#endif	/* NEWTESTCLASS2_H */

