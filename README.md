# 📡 Network-Multicasting

This project simulates a **Software Defined Networking (SDN) Controller** in a computer network environment, solving two graph-based problems using C++. The tasks focus on efficiently building **multicast trees** under bandwidth and cost constraints.

## 🔧 Features

### Problem 1: SDN Multicast with Partial Trees
Implements multicast tree construction under the following conditions:
- Each node in the graph is a destination.
- Partial trees are allowed.
- Bandwidth and cost constraints are enforced.

**Operations:**
- `insert(id, s, D, true, t)`
- `stop(id)`
- `rearrange()`

### Problem 2: SDN Multicast with Full Trees & Penalty Minimization
An extension where:
- Only a subset of nodes is targeted (`D ⊂ V`).
- Full multicast trees are mandatory.
- Penalties are incurred for unsatisfied requests.

**Operations:**
- `insert(id, s, D, false, t)` (returns `bool`)
- `stop(id)`
- `rearrange()`

## 🧠 Core Concepts

- **Graph Theory**: Networks are modeled as undirected graphs with bandwidth and cost per edge.
- **Multicast Tree**: Efficient structure for broadcasting from a single source to multiple destinations.

## 🧪 Test Cases

Test cases are crafted to challenge:
- Bandwidth limitations
- Rebuilding multicast trees on-the-fly
- Penalty trade-offs in request satisfaction

## 🛠️ Requirements
- C++20 compiler

## 📄 License
This project is part of a university course. Please do not copy or reuse code directly without proper acknowledgment.
