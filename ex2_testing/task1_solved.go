// Write a test for the function `abs` which performs a modulo operation
// i.e. it gets absolute value of the input argument
// Your task is to define the post-condition of this function
// and write a unit test that will check it

package main

import "fmt"

// ===========================================
// Test template
func testAbs(c int) bool {
	// The test should return "true" if it succeeds
	// and "false" if it fails

	//  you code goes here...
	return abs(c) >= 0
}

// ===========================================
// don't change anything below

func abs(r int) int {
	// returns absolute value of the input argument
	if r > 0 {
		return r
	} else {
		return r
	}
}

func check(testCase int, correct bool, text string) {
	if testAbs(testCase) == correct {
		fmt.Printf("%s test worked correctly\n", text)
	} else {
		fmt.Printf("%s test failed\n", text)
	}
}

func main() {
	check(1, true, "1")
	check(0, true, "2")
	check(-1, false, "3")
}
