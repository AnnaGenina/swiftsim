################################################################################
# This file is part of SWIFT.
# Copyright (c) 2018 Matthieu Schaller (matthieu.schaller@durham.ac.uk)
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
################################################################################

import re
import sys
import matplotlib
matplotlib.use("Agg")
from pylab import *

# Plot parameters
params = {'axes.labelsize': 10,
'axes.titlesize': 10,
'font.size': 12,
'legend.fontsize': 12,
'xtick.labelsize': 10,
'ytick.labelsize': 10,
'text.usetex': True,
 'figure.figsize' : (6.45,6.45),
'figure.subplot.left'    : 0.06,
'figure.subplot.right'   : 0.99,
'figure.subplot.bottom'  : 0.06,
'figure.subplot.top'     : 0.99,
'figure.subplot.wspace'  : 0.21,
'figure.subplot.hspace'  : 0.13,
'lines.markersize' : 6,
'lines.linewidth' : 3.,
'text.latex.unicode': True
}
rcParams.update(params)

threshold = 0.008

num_files = len(sys.argv) - 1

labels = ['Gpart assignment', 'Mesh comunication', 'Forward Fourier transform', 'Green function', 'Backwards Fourier transform',
          'Making gravity tasks', 'Splitting tasks', 'Counting and linking tasks', 'Setting super-pointers', 'Linking gravity tasks',
          'Creating send tasks', 'Exchanging cell tags', 'Creating recv tasks', 'Setting unlocks', 'Ranking the tasks', 'scheduler_reweight:', 
          'space_rebuild:', 'engine_drift_all:', 'engine_unskip:', 'engine_collect_end_of_step:', 'engine_launch:', 'writing particle properties', 
          'engine_repartition:', 'engine_exchange_cells:', 'Dumping restart files', 'engine_print_stats:', 'engine_marktasks:']
is_rebuild = [1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1]
times = np.zeros(len(labels))
counts = np.zeros(len(labels))

cols = ['0.5', '#332288', '#88CCEE', '#44AA99', '#117733', '#999933', '#DDCC77',
        '#CC6677', '#882255', '#AA4499', '#661100', '#6699CC', '#AA4466',
        '#4477AA']

total_time = 0
lastline = ''

for i in range(num_files):

    filename = sys.argv[i + 1]
    print "Analysing", filename

    # Open stdout file
    file = open(filename, 'r')

    # Search the different phrases
    for line in file:
    
        # Loop over the possbile labels
        for i in range(len(labels)):

            # Extract the different blocks
            if re.search("%s took"%labels[i], line):
                counts[i] += 1.
                times[i] += float(re.findall(r'[+-]?([0-9]*[.])?[0-9]+', line)[-1])

        # Find the last line with meaningful output (avoid crash report, batch system stuf....)
        if re.findall(r'\[[0-9]{4}\][ ]\[*', line) or re.findall(r'^\[[0-9]*[.][0-9]+\][ ]', line):
            lastline = line

    # Total run time
    total_time += float(re.findall(r'[+-]?([0-9]*[.])?[0-9]+', lastline)[1])
            
# Conver to seconds
times /= 1000.

# Total time
total_measured_time = np.sum(times)
print "\nTotal measured time: %.3f s"%total_measured_time

print "Total time:", total_time, "s\n"

# Ratios
time_ratios = times / total_time

# Better looking labels
for i in range(len(labels)):
    labels[i] = labels[i].replace("_", " ")
    labels[i] = labels[i].replace(":", "")
    labels[i] = labels[i].title()

times = np.array(times)
time_ratios = np.array(time_ratios)
is_rebuild = np.array(is_rebuild)

# Sort in order of importance
order = np.argsort(-times)
times = times[order]
time_ratios = time_ratios[order]
is_rebuild = is_rebuild[order]
labels = np.take(labels, order)

# Keep only the important components
important_times = [0.]
important_ratios = [0.,]
important_labels = ['Others (all below %.1f\%%)'%(threshold*100), ]
important_is_rebuild = [0] 
print "Elements in 'Other' category (<%.1f%%):"%(threshold*100)
for i in range(len(labels)):
    if time_ratios[i] > threshold:
        important_times.append(times[i])
        important_ratios.append(time_ratios[i])
        important_labels.append(labels[i])
        important_is_rebuild.append(is_rebuild[i])
    else:
        important_times[0] += times[i]
        important_ratios[0] += time_ratios[i]
        print " - '%-30s': %.4f%%"%(labels[i], time_ratios[i] * 100)

print "\nUnaccounted for: %.4f%%\n"%(100 * (total_time - total_measured_time) / total_time)

important_ratios = np.array(important_ratios)
important_is_rebuild = np.array(important_is_rebuild)

figure()

def func(pct):
    return "$%4.2f\\%%$"%pct

pie,_,_ = pie(important_ratios, explode=important_is_rebuild*0.2, autopct=lambda pct:func(pct), textprops=dict(color='0.1', fontsize=14), labeldistance=0.7, pctdistance=0.85, startangle=-15, colors=cols)
legend(pie, important_labels, title="SWIFT operations", loc="upper left")

savefig("time_pie.pdf", dpi=150)
