/*
 * Copyright (c) 2012, Joseph Max DeLiso
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are 
 * those of the authors and should not be interpreted as representing official 
 * policies, either expressed or implied, of the FreeBSD Project.
 */

#include "Elevator.hpp"
#include "Building.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/test/detail/unit_test_parameters.hpp>
#include <fstream>

namespace elevatorSim {

struct BuildingFixture {

   static Building* testBuilding;
   static const int testStoryCount;
   static const int testElevCount;

   BuildingFixture();
   ~BuildingFixture();
};

Building* BuildingFixture::testBuilding = NULL;
const int BuildingFixture::testStoryCount = 10;
const int BuildingFixture::testElevCount = 1;

BuildingFixture::BuildingFixture() {
   testBuilding = new Building(testStoryCount, testElevCount);
}

BuildingFixture::~BuildingFixture() {
   delete testBuilding;
}

} /* namespace elevatorSim */

BOOST_AUTO_TEST_SUITE( building_integration_tests )

BOOST_FIXTURE_TEST_CASE( 
   building_initialization_test, 
   elevatorSim::BuildingFixture ) {
      BOOST_REQUIRE_EQUAL(
         testBuilding->getStories(),
         testStoryCount );

      BOOST_REQUIRE_EQUAL(
         testBuilding->getMaxElev(),
         testElevCount );

      BOOST_REQUIRE_EQUAL(
         testBuilding->getFloors().size(),
         (unsigned int) testStoryCount);
}

BOOST_FIXTURE_TEST_CASE(
   building_memory_error_test, elevatorSim::BuildingFixture ) {

      const int TEST_ITER_COUNT = 1000;

      testBuilding->init();

      /* TODO: more checks */
}

BOOST_AUTO_TEST_SUITE_END()