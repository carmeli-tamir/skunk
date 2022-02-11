from setuptools import setup, find_packages

setup(name='skunk', version='0.1', packages=find_packages(), install_requires=["protobuf==3.15.0", "ioctl-opt==1.2.2"])