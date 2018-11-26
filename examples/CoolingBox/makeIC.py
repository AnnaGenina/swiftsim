###############################################################################
 # This file is part of SWIFT.
 # Copyright (c) 2016 Stefan Arridge (stefan.arridge@durhama.ac.uk)
 #                    Matthieu Schaller (matthieu.schaller@durham.ac.uk)
 # 
 # This program is free software: you can redistribute it and/or modify
 # it under the terms of the GNU Lesser General Public License as published
 # by the Free Software Foundation, either version 3 of the License, or
 # (at your option) any later version.
 # 
 # This program is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 # GNU General Public License for more details.
 # 
 # You should have received a copy of the GNU Lesser General Public License
 # along with this program.  If not, see <http://www.gnu.org/licenses/>.
 # 
 ##############################################################################

import h5py
import sys
from numpy import *

# Generates a SWIFT IC file with a constant density and pressure

# Parameters
periodic= 1           # 1 For periodic box
boxSize = 3.0857e21   # 1 kiloparsec    
rho = 2.36748e-25          # Density in cgs
P = 6.68e-12          # Pressure in cgs
gamma = 5./3.         # Gas adiabatic index
eta = 1.2349          # 48 ngbs with cubic spline kernel
fileName = "coolingBox.hdf5" 

if len(sys.argv) == 2:
	rho = float(sys.argv[1])
	P = float(sys.argv[2])

#---------------------------------------------------

# Read id, position and h from glass
glass = h5py.File("glassCube_16.hdf5", "r")
ids = glass["/PartType0/ParticleIDs"][:]
pos = glass["/PartType0/Coordinates"][:,:] * boxSize
h = glass["/PartType0/SmoothingLength"][:] * boxSize

# Compute basic properties
numPart = size(pos) / 3
mass = boxSize**3 * rho / numPart
internalEnergy = P / ((gamma - 1.) * rho)

#File
file = h5py.File(fileName, 'w')

# Header
grp = file.create_group("/Header")
grp.attrs["BoxSize"] = boxSize
grp.attrs["NumPart_Total"] =  [numPart, 0, 0, 0, 0, 0]
grp.attrs["NumPart_Total_HighWord"] = [0, 0, 0, 0, 0, 0]
grp.attrs["NumPart_ThisFile"] = [numPart, 0, 0, 0, 0, 0]
grp.attrs["Time"] = 0.0
grp.attrs["NumFilesPerSnapshot"] = 1
grp.attrs["MassTable"] = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
grp.attrs["Flag_Entropy_ICs"] = 0

# Runtime parameters
grp = file.create_group("/RuntimePars")
grp.attrs["PeriodicBoundariesOn"] = periodic

# Units
grp = file.create_group("/Units")
grp.attrs["Unit length in cgs (U_L)"] = 1.
grp.attrs["Unit mass in cgs (U_M)"] = 1.
grp.attrs["Unit time in cgs (U_t)"] = 1.
grp.attrs["Unit current in cgs (U_I)"] = 1.
grp.attrs["Unit temperature in cgs (U_T)"] = 1.

#Particle group
grp = file.create_group("/PartType0")

v  = zeros((numPart, 3))
ds = grp.create_dataset('Velocities', (numPart, 3), 'f')
ds[()] = v

m = full((numPart, 1), mass)
ds = grp.create_dataset('Masses', (numPart,1), 'f')
ds[()] = m

h = reshape(h, (numPart, 1))
ds = grp.create_dataset('SmoothingLength', (numPart, 1), 'f')
ds[()] = h

u = full((numPart, 1), internalEnergy)
ds = grp.create_dataset('InternalEnergy', (numPart,1), 'f')
ds[()] = u

ids = reshape(ids, (numPart, 1))
ds = grp.create_dataset('ParticleIDs', (numPart, 1), 'L')
ds[()] = ids

ds = grp.create_dataset('Coordinates', (numPart, 3), 'd')
ds[()] = pos

file.close()

print numPart
