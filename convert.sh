#!/bin/bash

echo "#declare defTexture1 = texture {"
echo "pigment { color rgb< 0.8, 0.6, 0.6 > }"
echo "finish { ambient 0.2 diffuse 0.5 }"
echo "}"

echo "#declare defTexture2 = texture {"
echo "pigment { color rgb< 0.8, 0.2, 0.2 > }"
echo "finish { ambient 0.2 diffuse 0.5 }"
echo "}"

echo "#declare defTexture3 = texture {"
echo "pigment { color rgb< 0.9, 0.9, 0.9 > }"
echo "finish { ambient 0.2 diffuse 0.5 }"
echo "}"

echo "#declare SHELL=mesh {"

(while read LINE; do
    echo "triangle {"
    echo $LINE                       | \
        sed -e "s:^:    :"             \
            -e "s:[{]Switch1\:0[}]:\n    texture { defTexture2 }\n:g"  \
            -e "s:[{]Switch1\:1[}]:\n    texture { defTexture1 }\n:g"  \
            -e "s:[{]Switch1\:2[}]:\n    texture { defTexture3 }\n:g"  |
        sed -e "s:\[[0-9,]*\]::g"      \
            -e "s:[\(][\(]*:<:g"       \
            -e "s:[\)][\)]*:>:g"
    echo "}"
done)
#    echo "    texture { defTexture }"

echo "}"
