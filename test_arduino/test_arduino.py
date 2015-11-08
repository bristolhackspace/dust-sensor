import unittest
import serial
import time
import logging
import math

logging.basicConfig(level=logging.DEBUG)

class TestArduino(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls._serial_port=serial.Serial()
        cls._serial_port.port='/dev/ttyACM0'
        cls._serial_port.timeout=1
        cls._serial_port.baudrate=115200
        cls._serial_port.open()
        time.sleep(2);

    @classmethod
    def tearDownClass(cls):
        cls._serial_port.close()

    def do_sum(self, num):
        ratio = num
        return 1.1*math.pow(ratio,3)-3.8*math.pow(ratio,2)+520*ratio+0.62; 

    def test_sum(self):
        i =52 
        j = 0
        max_err = 0
        err_ind = 0

        while i < 54:
            self._serial_port.write(str(i) + "\n")
            response = self._serial_port.readline()
            response = float(response.strip())
            error = abs(self.do_sum(i) - response)
            self.assertLess(error, 2)
            if error > max_err:
                max_err = error
                err_ind = i

            if j % 20 == 0:
                logging.info("%16.2f : %16.2f -> %16.2f err = %1.5f" % (i, self.do_sum(i), response, error))

            i += 0.01
            j += 1

        logging.info("max err %s at %s" % (max_err, err_ind))


if __name__ == '__main__':
    unittest.main()

