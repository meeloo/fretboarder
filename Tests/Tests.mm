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

std::string filePath(const std::string& path) {
    NSString* p = [[NSBundle bundleForClass:Tests.class] resourcePath];
    std::string pp = p.UTF8String;
    pp += "/" + path;
    return pp;
}


@implementation Tests

- (void)setUp {
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
}

- (void)testRightHanded {
    Instrument instrument;
    Fretboard fretboard(instrument);
    
    XCTAssertEqual(fretboard.fret_slots().size(), 25);
    XCTAssertEqual(fretboard.strings().front().distance_from_bridge(0), instrument.scale_length[1]);
    XCTAssertEqual(fretboard.strings().back().distance_from_bridge(0), instrument.scale_length[0]);
}

- (void)testLeftHanded {
    Instrument instrument;
    instrument.right_handed = false;
    Fretboard fretboard(instrument);
    
    XCTAssertEqual(fretboard.fret_slots().size(), 25);
    XCTAssertEqual(fretboard.strings().front().distance_from_bridge(0), instrument.scale_length[0]);
    XCTAssertEqual(fretboard.strings().back().distance_from_bridge(0), instrument.scale_length[1]);
}

- (void)testPrependicularFretAtBridge {
    Instrument instrument;
    instrument.perpendicular_fret_index = 100;
    Fretboard fretboard(instrument);
    
    XCTAssertEqual(fretboard.fret_slots().size(), 25);
    XCTAssertEqual(fretboard.strings().front().distance_from_bridge(0), instrument.scale_length[1]);
    XCTAssertEqual(fretboard.strings().back().distance_from_bridge(0), instrument.scale_length[0]);
}

- (void)testPrependicularFretAtNut {
    Instrument instrument;
    instrument.perpendicular_fret_index = 0;
    Fretboard fretboard(instrument);
    
    XCTAssertEqual(fretboard.fret_slots().size(), 25);
    XCTAssertEqual(fretboard.strings().front().distance_from_bridge(0), instrument.scale_length[1]);
    XCTAssertEqual(fretboard.strings().back().distance_from_bridge(0), instrument.scale_length[0]);
}

- (void)testStringPerpendicularFretAtNut {
    double scale = 635;
    double nut_dist = 10;
    String string(0, scale, 0, 0, 0, true, nut_dist);
    printf("X at start: %lf\n", string.x_at_start());
    printf("X at nut: %lf\n", string.x_at_nut());
    printf("X at bridge: %lf\n", string.x_at_bridge());

    XCTAssertEqual(string.x_at_bridge() - string.x_at_start(), scale);
    XCTAssertEqual(string.x_at_nut(), string.x_at_start() - nut_dist);
}


- (void)testStringPerpendicularFretAtBridge {
    double scale = 635;
    double nut_dist = 10;
    String string(0, scale, 0, 0, 0, true, nut_dist);
    printf("X at start: %lf\n", string.x_at_start());
    printf("X at nut: %lf\n", string.x_at_nut());
    printf("X at bridge: %lf\n", string.x_at_bridge());

    XCTAssertEqual(string.x_at_bridge() - string.x_at_start(), scale);
    XCTAssertEqual(string.x_at_nut(), string.x_at_start() - nut_dist);
}

- (void)testStringPerpendicularFretBehindNut {
    double scale = 635;
    double nut_dist = 10;
    String string(0, scale, -3, 0, 0, true, nut_dist);
    printf("X at start: %lf\n", string.x_at_start());
    printf("X at nut: %lf\n", string.x_at_nut());
    printf("X at bridge: %lf\n", string.x_at_bridge());
    
    XCTAssertEqualWithAccuracy(string.x_at_bridge() - string.x_at_start(), scale, 0.0000000000000000000000000001);
    XCTAssertEqual(string.x_at_nut(), string.x_at_start() - nut_dist);
}

- (void)testLoadFromJSon {
    Instrument instrument;
    
    XCTAssert(instrument.load(filePath("default.frt")));
}

- (void)testOneStringFromDefault {
    Instrument instrument;
    XCTAssert(instrument.load(filePath("default.frt")));
    instrument.number_of_strings = 1;
    Fretboard fretboard(instrument);
    
}


- (void)testBreakingPerpendicularFreboard1 {
    Instrument instrument;
    
    XCTAssert(instrument.load(filePath("breaking2.frt")));
    
    Fretboard fretboard(instrument);
    auto p0 = fretboard.board_shape().points[1];
    auto p1 = fretboard.board_shape().points[2];
    double dist = p0.distanceFrom(p1);
    XCTAssertGreaterThan(dist, 0);
}

- (void)testLineIntersection1 {
    fretboarder::Point p0(0, 0);
    fretboarder::Point p1(1, 1);
    fretboarder::Point p2(1, 0);
    fretboarder::Point p3(0, 1);
    
    fretboarder::Vector v0(p0, p1);
    fretboarder::Vector v1(p2, p3);
    
    fretboarder::Point r = v0.intersection(v1);
    fretboarder::Point e(0.5, 0.5);
    
    XCTAssert(r == e);
}

- (void)testLineIntersection2 {
    fretboarder::Point p0(0, 0);
    fretboarder::Point p1(10, 10);
    fretboarder::Point p2(10, 0);
    fretboarder::Point p3(0, 10);
    
    fretboarder::Vector v0(p0, p1);
    fretboarder::Vector v1(p2, p3);
    
    fretboarder::Point r = v0.intersection(v1);
    fretboarder::Point e(5, 5);
    
    XCTAssert(r == e);
}

- (void)testLineIntersection3 {
    fretboarder::Point p0(0, 0);
    fretboarder::Point p1(1, 10);
    fretboarder::Point p2(1, 0);
    fretboarder::Point p3(0, 10);
    
    fretboarder::Vector v0(p0, p1);
    fretboarder::Vector v1(p2, p3);
    
    fretboarder::Point r = v0.intersection(v1);
    fretboarder::Point e(0.5, 5);
    
    XCTAssert(r == e);
}

- (void)testLineOffset1 {
    fretboarder::Point p0(0, -1);
    fretboarder::Point p1(0, 1);

    fretboarder::Point p2(10, -1);
    fretboarder::Point p3(10, 1);


    fretboarder::Vector v0(p0, p1);

    Vector r = v0.offset2D(10);

    XCTAssert(p2 == r.point1);
    XCTAssert(p3 == r.point2);
}

- (void)testLineOffset2 {
    fretboarder::Point p0(1, 0);
    fretboarder::Point p1(-1, 0);

    fretboarder::Point p2(1, 10);
    fretboarder::Point p3(-1, 10);


    fretboarder::Vector v0(p0, p1);

    Vector r = v0.offset2D(10);

    XCTAssert(p2 == r.point1);
    XCTAssert(p3 == r.point2);
}



//- (void)testPerformanceExample {
//    // This is an example of a performance test case.
//    [self measureBlock:^{
//        // Put the code you want to measure the time of here.
//    }];
//}

@end
