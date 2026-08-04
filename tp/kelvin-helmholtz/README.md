# Kevin-Helmholtz Instability

## Introduction

The Kelvin-Helmholtz instability is a fluid dynamics phenomenon that occurs at the interface of two fluids moving at different velocities. This instability leads to the formation of characteristic wave patterns and vortices.

This project aims to simulate the Kelvin-Helmholtz instability using numerical methods and parallel computing techniques. For this aim, we use the Vorticity-Streamfunction Formulation of 2D Navier-Stokes equations.

The 2D Navier-Stokes equations in the vorticity-streamfunction formulation are given by:

$$
\frac{\partial \omega}{\partial t} + (\mathbf{u} \cdot \nabla) \omega = \nu \nabla^2 \omega
$$

where:

- $\omega$ is the vorticity
- $\mathbf{u} = (u_x, u_y)$ is the velocity field
- $\nu$ is the kinematic viscosity.

The streamfunction $\psi$ is related to the velocity field by:

$$
\nabla^2 \psi = -\omega
$$
`
And the velocity components can be expressed in terms of the streamfunction as:

$$
u_x = \frac{\partial \psi}{\partial y}, \quad u_y = -\frac{\partial \psi}{\partial x}
$$

The vorticity can also be expressed in terms of the velocity field as:

$$
\omega = \nabla\times \mathbf{u} = \frac{\partial u_y}{\partial x} - \frac{\partial u_x}{\partial y}
$$

This equation can be decomposed into 3 components:

- the time derivative of the vorticity $\frac{\partial \omega}{\partial t}$,
- the advection term $A = (\mathbf{u} \cdot \nabla) \omega$,
- the diffusion term $D = \nu \nabla^2 \omega$.

Solving numerically these equations implies the following steps:

1. Compute the vorticity field $\omega$ at each time step by solving the vorticity transport equation. For this purpose, we use a finite difference method to discretize the spatial derivatives and an explicit Euler time-stepping scheme to advance the solution in time.

$$ \omega^{n+1} = \omega^n + \Delta t \left( A^n +  D^n \right) $$

The advection term $A^n$ is computed using a central difference scheme:

$$A^n = - \left( u_x^n \frac{\partial \omega^n}{\partial x} + u_y^n \frac{\partial \omega^n}{\partial y} \right)$$

Same for the diffusion term $D^n$:

$$D^n = \nu \left( \frac{\partial^2 \omega^n}{\partial x^2} + \frac{\partial^2 \omega^n}{\partial y^2} \right)$$

2. Compute the streamfunction $\psi$ by solving the Poisson equation $\nabla^2 \psi = -\omega$. We use a Poisson solver based on the Fast Fourier Transform (FFT) to efficiently solve this equation in the frequency domain.

3. Compute the velocity field $\mathbf{u}$ from the streamfunction $\psi$ using the relations $u_x = \frac{\partial \psi}{\partial y}$ and $u_y = -\frac{\partial \psi}{\partial x}$. We use central difference schemes to approximate the spatial derivatives.

The following video illustrates the evolution of the Kelvin-Helmholtz instability over time, showing the formation of characteristic wave patterns and vortices as the two fluids interact.

<video controls src="assets/animation.mp4" title="Title"></video>

## Description of the project

### Sequential Code

A sequential version of the code is present at `python/seq/main.py`.