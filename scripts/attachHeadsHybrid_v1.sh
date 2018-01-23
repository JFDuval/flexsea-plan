#!/bin/bash

# To run this script on Windows, simply call "./pullAllSubmodulesHybrid_v1.sh"

echo 
echo [This script will checkout the dev or master branch of the 5 flexsea-plan submodules]
echo 

echo flexsea-comm...
cd flexsea-comm
git checkout dev
cd ..
echo 

echo flexsea-system...
cd flexsea-system
git checkout dev
cd ..
echo 

echo flexsea-shared...
cd flexsea-shared
git checkout dev
cd ..
echo 

echo flexsea-projects...
cd flexsea-projects
git checkout master
cd ..
echo 

echo [Done]
echo 