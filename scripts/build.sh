#!/bin/bash

python3 scripts/generate-graphics.py
python3 scripts/generate-scenes.py
python3 scripts/generate-form.py

make -j$(nproc)