/*
 * File:   newtestclass2.cpp
 * Author: staszek
 *
 * Created on 2012-08-07, 22:10:52
 */

#include "newtestclass2.h"


CPPUNIT_TEST_SUITE_REGISTRATION(newtestclass2);

newtestclass2::newtestclass2() {
}

newtestclass2::~newtestclass2() {
}

void newtestclass2::setUp() {
}

void newtestclass2::tearDown() {
}

void newtestclass2::testMethod() {
    CPPUNIT_ASSERT(true);
}

void newtestclass2::testFailedMethod() {
    CPPUNIT_ASSERT(false);
}

