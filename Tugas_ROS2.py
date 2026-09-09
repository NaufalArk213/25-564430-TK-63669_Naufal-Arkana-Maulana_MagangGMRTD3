#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
import time

class MoveTurtleSquare(Node):
    def __init__(self):
        super().__init__('move_turtle_square')
        self.publisher = self.create_publisher(Twist, '/turtle1/cmd_vel', 10)
        self.get_logger().info("Memulai kontrol otomatis TurtleSim membentuk persegi...")
        time.sleep(1)

    def move_forward(self, speed=2.0, duration=2.0):
        cmd = Twist()
        cmd.linear.x = float(speed)
        
        start = time.time()
        while time.time() - start < duration:
            self.publisher.publish(cmd)
            time.sleep(0.05)
            
        cmd.linear.x = 0.0
        self.publisher.publish(cmd)
        time.sleep(0.3)

    def turn_90_degrees(self, speed=1.5708, duration=1.0):
        cmd = Twist()
        cmd.angular.z = float(speed)
        
        start = time.time()
        while time.time() - start < duration:
            self.publisher.publish(cmd)
            time.sleep(0.05)
            
        cmd.angular.z = 0.0
        self.publisher.publish(cmd)
        time.sleep(0.3)

    def run(self):
        for i in range(4):
            self.get_logger().info(f"[Sisi {i+1}/4] Maju...")
            self.move_forward()
            
            self.get_logger().info(f"[Sudut {i+1}/4] Belok 90 derajat...")
            self.turn_90_degrees()
            
        self.get_logger().info("Selesai! Perjalanan membentuk lintasan persegi berhasil dibuat.")

def main(args=None):
    rclpy.init(args=args)
    turtle = MoveTurtleSquare()
    turtle.run()
    turtle.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
