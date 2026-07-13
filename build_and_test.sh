#!/bin/bash
set -e

echo "==> Building MiniRBS server..."
make

echo "==> Building and running unit tests..."
make test

echo "==> All good — build and tests passed."