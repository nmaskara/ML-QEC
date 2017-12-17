# ML-QEC
Written by Nishad Maskara, 2017

This repository contains code written for applying machine learning to quantum error correction. 

A Topological code can be implemented by extending the Base_Lattice.hpp class.
The topological code's I have implemented are:
Toric code (Square, Triangle, Hexagonal)
Surface code, the toric code but with boundaries. (Surface)
6.6.6 Color code on a torus (Color Code)
6.6.6 Color code with boundaries, on a triangle (Color Code2)
The surface code with a twist, as described by https://arxiv.org/pdf/1612.04795.pdf (Twist)

A neural networkk classifier can be trained to decode any topological code using the trainmodel python program. I have implemented a fully connected classifier, but different architectures can be tested by modifying the makeModel function. I have written a variety of scripts to systematically train networks with a variety of hyperparameters, these are located in the tests folder.
