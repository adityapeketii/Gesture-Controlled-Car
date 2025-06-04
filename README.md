# 🤖 Hand Gesture Controlled Car using Arduino

## 📝 Introduction

This project presents a **hand gesture-controlled car** that leverages motion sensors and wireless communication to create a more **intuitive and immersive** method of controlling a vehicle. Unlike traditional remote-controlled systems, this project uses hand gestures to drive the car, offering a seamless and modern interaction method.

---

## 🎯 Objectives

- ✅ Develop an efficient gesture recognition algorithm.
- ✅ Implement reliable wireless communication.
- ✅ Integrate multiple hardware components.
- ✅ Optimize system performance and responsiveness.
- ✅ Evaluate the system under real-world conditions.

---

## ⚙️ Components Used

| Component | Description |
|----------|-------------|
| **Arduino UNO & NANO** | UNO controls the car; NANO handles gesture sensing. |
| **nRF24L01 Modules** | Wireless communication modules between controller and car. |
| **MPU6050** | 6-axis accelerometer and gyroscope for gesture detection. |
| **L298N Motor Driver** | Dual H-Bridge to control the car’s DC motors. |
| **Lithium-Ion Batteries** | Power supply for car and gesture controller. |

---

## 📡 System Architecture

- **Transmitter**: Hand-mounted MPU6050 connected to Arduino Nano + nRF24L01 (TX)
- **Receiver**: Arduino Uno in car connected to nRF24L01 (RX), L298N motor driver, and motors

---

## 🧑‍💻 Contributors

- M V S Navaneeth – EC22B1038  
- P. V. Aditya Vardhan – EC22B1045


