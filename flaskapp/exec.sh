#!/bin/bash
cd
cd ./flask-env 
source bin/activate
cd
cd ./projects/flaskapp
python3 app.py &
sleep 2
firefox localhost:5000
