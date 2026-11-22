# PCA Visualizer

A step-by-step **Principal Component Analysis (PCA) visualizer** built in C++17 using Raylib.

## About

I started this project to understand the mathematics behind PCA rather than relying on an existing library. My goal is to implement the algorithm from scratch and visualize each stage to make it easier to understand.

I'm also using this project to learn **GoogleTest**, practice test-driven development (TDD), and improve how I structure and test my code.

**Note:** I'm using AI to help build the Raylib interface and handle some boilerplate. The mathematics, PCA implementation, and core logic are written by me.

## What's Implemented

- Custom `Matrix` class with the necessary matrix operations.
- Data centering and covariance matrix calculation.
- Eigendecomposition of symmetric 2×2 matrices.
- Projection onto one or two principal components.
- Reconstruction of the original data from PCA coordinates.
- `PCA` class that brings the mathematical operations together.
- GoogleTest unit tests for the mathematical backend.
- Raylib interface for visualizing the PCA process step by step.
- CSV loading for custom datasets.

Currently, the implementation supports **2D datasets with one or two principal components**.

## Project Structure

The code is separated into four main parts:

- **Matrix:** Matrix representation and operations.
- **Statistics:** Mathematical functions used by PCA.
- **PCA:** Fitting, projection, and reconstruction.
- **UI:** Raylib interface and visualization.

## Building and Testing

The project uses **CMake, GoogleTest, and CTest**, with a Makefile to simplify common commands.

```bash
make configure   # Configure the project
make build       # Build the application
make test        # Build and run tests
make clean       # Remove build files
```

## What's Next

- Explained variance and reconstruction error visualization.
- UI improvements and clearer step-by-step explanations.
- Additional testing and numerical stability improvements.

The project is still a work in progress. For now, I'm focusing on understanding and correctly implementing 2D PCA before considering support for higher-dimensional datasets