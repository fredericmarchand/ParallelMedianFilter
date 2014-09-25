#!/bin/bash

echo "Test 1: "
python test.py output1 temp1
diff output1 temp1
echo "Test 2: "
python test.py output2 temp2
diff output2 temp2
echo "Test 3: "
python test.py output3 temp3
diff output3 temp3
echo "Test 4: "
python test.py output4 temp4
diff output4 temp4
echo "Test 5: "
python test.py output5 temp5
diff output5 temp5

