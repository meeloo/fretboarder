//
//  Tests.mm
//  Tests
//
//  Created by Sebastien Metrot on 17/05/2020.
//  Copyright © 2020 Autodesk. All rights reserved.
//

#import <XCTest/XCTest.h>

#include "Fretboard.hpp"
#include "String.hpp"
#include "Geometry.hpp"

using namespace fretboarder;

@interface Tests : XCTestCase

@end

@implementation Tests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testExample {
    // This is an example of a functional test case.
    // Use XCTAssert and related functions to verify your tests produce the correct results.
    
    Instrument instrument;
    Fretboard fretboard(instrument);
    
    XCTAssertEqual(fretboard.fret_slots().count, 25);
}

//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
