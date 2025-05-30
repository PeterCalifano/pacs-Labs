---
marp: true
#size: 4:3
style: |
    img[alt~="center"] {
      display: block;
      margin: 0 auto;
    }
---

# **Laboratory 16**
## MPI & C++23 multi-thread + Jupyter notebooks

### Paolo Joseph Baioni

### 30/05/2025


---
# Outline
- recalls on spack and prerequisites
- application tour
- MPI & C++23 multi-thread
- notebook example

---
# Prerequisites

```
source /etc/profile.d/modules.sh
source spack-0.23/share/spack/setup-env.sh
module load gcc/14.2.0
module load intel-oneapi-tbb/2022
module load openmpi/5.0.5
```
See previous lab for detailed instructions; if you haven't installed modules, you can skip the first line above and use `spack load` instead. In any case, use tab-completion for the full signature of each module, eg
`module load gcc/14.2.0-gcc-13.3.0-hcg2eiv`

---
# Application: 2D heat equation
## Boundary and initial conditions
![w:600 center](img/bc+ic.png)

---
# Application: 2D heat equation
## Grid & update
$$
T(i,j;t) =  \sum_{m=i-1, n=j-1}^{m=i+1, n=j+1} T(m,n;t)
$$
![bg left 85%](img/grid.png)

---
# Application: 2D heat equation
## Halo exchange
![w:900 center](img/halo_exchange.png)

---
# Application: 2D heat equation
## Expected output
![w:500 center](img/output.png)

---
# MPI in a multi-threaded environment
 - [https://docs.open-mpi.org/en/main/man-openmpi/man3/MPI_Init_thread.3.html](https://docs.open-mpi.org/en/main/man-openmpi/man3/MPI_Init_thread.3.html) 

---
# Parallel I/O
 - [https://docs.open-mpi.org/en/main/man-openmpi/man3/MPI_File_open.3.html](https://docs.open-mpi.org/en/main/man-openmpi/man3/MPI_File_open.3.html)
 - [https://docs.open-mpi.org/en/main/man-openmpi/man3/MPI_File_set_size.3.html](https://docs.open-mpi.org/en/main/man-openmpi/man3/MPI_File_set_size.3.htmls)
 - [https://docs.open-mpi.org/en/main/man-openmpi/man3/MPI_File_iwrite_at.3.html](https://docs.open-mpi.org/en/main/man-openmpi/man3/MPI_File_iwrite_at.3.html)

---
# C++ concurrency (11) and thread (20) support libraries
 - [https://en.cppreference.com/w/cpp/header/thread.html](https://en.cppreference.com/w/cpp/header/thread.html)
 - [https://en.cppreference.com/w/cpp/header/atomic.html](https://en.cppreference.com/w/cpp/header/atomic.html)
 - [https://en.cppreference.com/w/cpp/header/barrier.html](https://en.cppreference.com/w/cpp/header/barrier.html)

---
# Synchronisation in distributed vs shared memory
`MPI_Barrier()` and `std::barrier::arrive_and_wait()` both serve the purpose of synchronisation; however, `MPI_Barrier()` is designed for process synchronisation across multiple nodes in a distributed computing environment, whereas `std::barrier::arrive_and_wait()` is used for synchronising threads within a single process.  

While a single call to `MPI_Barrier()` is typically sufficient to synchronise all processes, in a multithreaded environment, it may be necessary to call `std::barrier::arrive_and_wait()` once (or more, depending on the specific computations) for each thread.

 ---
 # Jupyter notebooks
 ```
 python -m venv plot
 source plot/bin/activate
 pip install --upgrade pip
 pip install numpy matplotlib jupyter
 jupyter notebook&
 ```
 `deactivate` when finished

---
# References
 - [Porting a scientific application to GPU using C++ standard parallelism](https://www.researchgate.net/publication/356217103_Porting_a_scientific_application_to_GPU_using_C_standard_parallelism) and references thereinafter
 - ranges before c++23: [https://github.com/ericniebler/range-v3](https://github.com/ericniebler/range-v3)
 - c++26 proposal: [https://github.com/brycelelbach/wg21_p2300_execution](https://github.com/brycelelbach/wg21_p2300_execution), [https://en.cppreference.com/w/cpp/experimental/execution.html](https://en.cppreference.com/w/cpp/experimental/execution.html) and [https://github.com/NVIDIA/stdexec](https://github.com/NVIDIA/stdexec)
 - [https://jupyter.org/](https://jupyter.org/)
