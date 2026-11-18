# PCA Visualizer

A Principal Component Analysis (PCA) visualizer built entirely in C++ using Raylib.

## Goal of the Project

The goal of this project is to learn the fundamentals of PCA, its applications, and the mathematics behind the matrix operations involved.

I want to understand how the algorithm works internally, rather than simply using an existing PCA library. To achieve this, I'll implement the algorithm myself and build a step-by-step visualizer that shows each stage of the process, hopefully making it easier for others to understand as well.

**Note:** I won't be implementing the Raylib UI entirely by hand. I'll use AI to help with the interface and some of the boilerplate code. The underlying mathematics, PCA implementation, and core logic will be written by me.

## Planned Features

- CSV file loading, allowing users to experiment with their own datasets.
- Support for an arbitrary number of features, with data visualized on a 2D plane.
- A step-by-step view of the PCA algorithm, explaining what happens at each stage.
- Visualization of important intermediate results, such as the covariance matrix, eigenvalues, eigenvectors, and explained variance.

## Initial Project Structure

To keep the code organized and modular, I'll separate the main responsibilities into different files:

- **Matrix:** A matrix of doubles with the necessary operations.
- **Dataset:** Stores the data and feature names (headers). Can load data from a CSV file through its constructor.
- **PCA:** Implements the PCA algorithm.
- **PCAResult:** Stores the results obtained at each step of the PCA algorithm, so they can be displayed step by step.
- **UI:** Handles the Raylib interface and visualization.
- **Maths:** Any math helpers will be stored inside math_utils. 
