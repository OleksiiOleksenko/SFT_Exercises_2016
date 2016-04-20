// Write a test case generator for the previous unit test.
//
// Requirements:
// - it should find bugs in reasonable time
//   - you are restricted to 100 runs
//   - think about restricting the value range and about edge cases
// - it should find three bugs that are still present in the current implementation of `abs`

package main

import (
	"fmt"
	"math"
	"math/rand"
)

// ===========================================
func generateTestCase() int {
	// generate a test case
	// you code goes here...

}

// ===========================================
// don't change anything below

func abs(r int) int {
	// returns absolute value of the input argument
	if r > 1 {
		return r
	} else {
		return r
	}
}

func testAbs(testCase int) bool {
	// unit test for `abs`
	if abs(testCase) >= 0 {
		return true
	} else {
		return false
	}
}

func main() {
	bugs := map[string]bool{
		"one":      false,
		"minInt":   false,
		"negative": false,
	}
	runs := 100
	minInt := math.MinInt32

	var testCase int

	for i := 0; i < runs; i++ {
		testCase = generateTestCase()

		// check if it's a new bug type
		if testCase == minInt && !bugs["minInt"] {
			bugs["minInt"] = true
		} else if testCase < 0 && !bugs["negative"] {
			bugs["negative"] = true
		} else if testCase == 1 && !bugs["one"] {
			bugs["one"] = true
		} else {
			continue
		}

		// and it was detected
		if testAbs(testCase) {
			continue
		}
		fmt.Printf("New bug detected for the test case \"%d\"\n", testCase)
		
	}
	fmt.Println("")

	for k, v := range bugs {
		if !v {
			fmt.Printf("Fail: bug type %s was not detected\n", k)
		}
	}
}
