#!/bin/bash
ts=$(gdate +%s%N) ; $@ ; tt=$((($(gdate +%s%N) - $ts)/1000000)) ; echo "Time taken: $tt milliseconds"
