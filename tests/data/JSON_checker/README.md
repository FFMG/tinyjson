# TinyJSON

TinyJSON is a fast and small C++ JSON parser

## JSON Checker

This test suit comes from the [JSON Checker](https://www.json.org/JSON_checker/).

Basically, (in their own words), "*JSON_checker comes with a test suite. If the JSON_checker is working correctly, it must accept all of the pass*.json files and reject all of the fail*.json files.*".

We use it exactly the same way, we parse all the files and pass/fail if we get any errors, the output itself is not really validated.
