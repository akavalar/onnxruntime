# ONNX Runtime and Python 2.7

## Updated for 0.4.0 (July 3rd, 2019)
To build the latest ONNX Runtime (0.4.0 as of July 3rd, latest commit e3919d3fcecbdd80f017f8176d4d5435d05a4d38) for use with Python 2.7 on Ubuntu 18.04 LTS, I followed the instructions below with some tweaks here and there:
- the testing script (onnxruntime_test_python.py) has been changed a bit (make sure you account for all tests involving strings!)
- four test cases instead of three in onnxruntime_test_python_backend.py
- some minimal changes to build.py (requiring additional "which" and "Path")

Also, I came across the same issue as reported here: https://github.com/microsoft/onnxruntime/issues/1258

## Requirements

1. Python modules
	* `pip install backports.shutil_which`
	* `pip install pathlib2`
	* `pip install numpy`
	* `pip install more_itertools==5.0.0` (see below)
	* `pip install pytest`
2. CMake
3. zip (if on Ubuntu 16.04)
4. If you want to test the compiled ONNX Runtime against additional ONNX models:
	* `pip install onnx`
	* locate the `__init__.py` file in `/home/username/.local/lib/python2.7/site-packages/onnx/backend/test/runner` (pip) or `/home/username/.conda/envs/yourenv/lib/python2.7/site-packages/onnx/backend/test/runner` (conda)
	* modify the following two lines:
		1. `def run(test_self, device):  # type: (Any, Text) -> None` => `def run(test_self, device, modeltype="bytes"):  # type: (Any, Text, Text) -> None`
		2. `prepared_model = self.backend.prepare(model, device)` => `prepared_model = self.backend.prepare(model, modeltype, device)`
	* see https://github.com/onnx/onnx/compare/master...akavalar:master
5. Build
	* `git clone https://github.com/akavalar/onnxruntime`
	* `./build.sh --config RelWithDebInfo --build_wheel --cmake_path '/home/username/cmake-3.13.4-Linux-x86_64/bin/cmake' --ctest_path '/home/username/cmake-3.13.4-Linux-x86_64/bin/ctest'`
6. Install & get the rest of the ONNX suite
	* `pip install onnxruntime-0.3.0-cp27-cp27mu-linux_x86_64.whl`
	* `pip install onnx` (if you didn't do it in #4)
	* `pip install skl2onnx`

List of all changes: http://github.com/Microsoft/onnxruntime/compare/master...akavalar:master

## Notes:
1. When starting a session, you need to specify whether you're loading the model from disk (use argument `modeltype="path"`) or if the model is already loaded in memory (use argument `modeltype="bytes"`). In Python 2 byte and string types are the same, so the required `modeltype` argument specifies the type instead.
2. If the build errors out in the `onnx_backend_test_series.py` section when running ONNX tests (#4 above), reinstall `more-itertools` and try again (see https://stackoverflow.com/questions/54648246/invalid-syntax-in-more-itertools-when-running-pytest).
3. Pybind11 documentation (https://pybind11.readthedocs.io/en/stable/advanced/cast/strings.html) recommends using unicode_literals (https://python-future.org/unicode_literals.html).
4. All strings need to be Unicode strings (not byte strings) and they can only contain ASCII characters. If not, you will __very likely__ end up with incorrect results. Some examples (data found in `/onnxruntime/onnxruntime/test/testdata`):

```
import onnxruntime as onnxrt
import numpy as np
```

#1
```
sess = onnxrt.InferenceSession("/home/username/LabelEncoder.pb", modeltype="path")
input_name = sess.get_inputs()[0].name
output_name = sess.get_outputs()[0].name
	
x = np.array([u'4'])
#x = np.array(['4'], dtype=np.object) # works too
#x = np.array(['4']) # doesn't work

res = sess.run([output_name], {input_name: x})
np.testing.assert_allclose(np.array([3], dtype=np.int64), res[0], rtol=1e-05, atol=1e-08)
```

#2
```
sess = onnxrt.InferenceSession("/home/username/identity_string.pb", modeltype="path")
x_name = sess.get_inputs()[0].name
output_name = sess.get_outputs()[0].name

x = np.array(['this', 'is', 'identity', 'test'], dtype=np.unicode).reshape((2,2))
#x = np.array(['this', 'is', 'identity', 'test'], dtype=np.object).reshape((2,2)) # works
#x = np.array([u'this', u'is', u'identity', u'test']).reshape((2,2)) # works too
#x = np.array(['this', 'is', 'identity', 'test'], dtype=np.str).reshape((2,2)) # doesn't work

res = sess.run([output_name], {x_name: x})
np.testing.assert_equal(x, res[0])
```

#3
```
sess = onnxrt.InferenceSession("/home/username/identity_string.pb", modeltype="path")
x_name = sess.get_inputs()[0].name
output_name = sess.get_outputs()[0].name

x = np.array(['Olá', '你好', '여보세요', 'hello'], dtype=np.object).reshape((2,2)) # works (watch out!)
#x = np.array(['Olá', '你好', '여보세요', 'hello'], dtype=np.unicode).reshape((2,2)) # doesn't work
#x = np.array([u'Olá', u'你好', u'여보세요', u'hello'], dtype=np.object).reshape((2,2)) # segfault
#x = np.array(['Olá', '你好', '여보세요', 'hello'], dtype=np.str).reshape((2,2)) # doesn't work

res = sess.run([output_name], {x_name: x})
np.testing.assert_equal(x, res[0])
```

<p align="center"><img width="50%" src="docs/images/ONNX_Runtime_logo_dark.png" /></p>

[![Build Status](https://dev.azure.com/onnxruntime/onnxruntime/_apis/build/status/Windows%20CPU%20CI%20Pipeline?label=Windows+CPU)](https://dev.azure.com/onnxruntime/onnxruntime/_build/latest?definitionId=9)
[![Build Status](https://dev.azure.com/onnxruntime/onnxruntime/_apis/build/status/Windows%20GPU%20CI%20Pipeline?label=Windows+GPU)](https://dev.azure.com/onnxruntime/onnxruntime/_build/latest?definitionId=10)
[![Build Status](https://dev.azure.com/onnxruntime/onnxruntime/_apis/build/status/Linux%20CPU%20CI%20Pipeline?label=Linux+CPU)](https://dev.azure.com/onnxruntime/onnxruntime/_build/latest?definitionId=11)
[![Build Status](https://dev.azure.com/onnxruntime/onnxruntime/_apis/build/status/Linux%20GPU%20CI%20Pipeline?label=Linux+GPU)](https://dev.azure.com/onnxruntime/onnxruntime/_build/latest?definitionId=12)
[![Build Status](https://dev.azure.com/onnxruntime/onnxruntime/_apis/build/status/MacOS%20CI%20Pipeline?label=MacOS+CPU)](https://dev.azure.com/onnxruntime/onnxruntime/_build/latest?definitionId=13)

**ONNX Runtime** is an open-source scoring engine for Open Neural Network Exchange (ONNX) models.

ONNX is an open format for machine learning (ML) models that is supported by various ML and DNN frameworks and tools. This format makes it easier to interoperate between frameworks and to maximize the reach of your hardware optimization investments. Learn more about ONNX on [https://onnx.ai](https://onnx.ai) or view the [Github Repo](https://github.com/onnx/onnx).

# Why use ONNX Runtime
ONNX Runtime is an open architecture that is continually evolving to adapt to and address the newest developments and challenges in AI and Deep Learning. We will keep ONNX Runtime up to date with the ONNX standard, supporting all ONNX releases with future compatibliity while maintaining backwards compatibility with prior releases.

ONNX Runtime continuously strives to provide top performance for a broad and growing number of usage scenarios in Machine Learning. Our investments focus on these 3 core areas:
1. Run any ONNX model
2. High performance
3. Cross platform

## Run any ONNX model

### Alignment with ONNX Releases
ONNX Runtime provides comprehensive support of the ONNX spec and can be used to run all models based on ONNX v1.2.1 and higher. See ONNX version release details [here](https://github.com/onnx/onnx/releases).

As of January 2019, ONNX Runtime supports ONNX 1.3. We will soon add support for the recently released ONNX 1.4.

### Traditional ML support
ONNX Runtime fully supports the ONNX-ML profile of the ONNX spec for traditional ML scenarios.

## High Performance
You can use ONNX Runtime with both CPU and GPU hardware. You can also plug in additional execution providers to ONNX Runtime. With many graph optimizations and various accelerators, ONNX Runtime can often provide lower latency and higher efficiency compared to other runtimes. This provides smoother end-to-end customer experiences and lower costs from improved machine utilization.

Currently ONNX Runtime supports CUDA and MKL-DNN (with option to build with MKL) for computation acceleration. To add an execution provider, please refer to [this page](docs/AddingExecutionProvider.md).

We are continuously working to integrate new execution providers to provide improvements in latency and efficiency. We have ongoing collaborations to integrate the following with ONNX Runtime:
	* Intel MKL-DNN and nGraph
	* NVIDIA TensorRT

## Cross Platform
ONNX Runtime offers:
* APIs for Python, C#, and C
* Available for Linux, Windows, and Mac 

See API documentation and package installation instructions [below](#Installation).

Looking ahead: To broaden the reach of the runtime, we will continue investments to make ONNX Runtime available and compatible with more platforms. These include but are not limited to:
* C# for Mac
* [ARM](BUILD.md##arm-builds)

# Getting Started
If you need a model:
* Check out the [ONNX Model Zoo](https://github.com/onnx/models) for ready-to-use pre-trained models.
* To get an ONNX model by exporting from various frameworks, see [ONNX Tutorials](https://github.com/onnx/tutorials).

If you already have an ONNX model, just [install the runtime](#Installation) for your machine to try it out. One easy way to deploy the model on the cloud is by using [Azure Machine Learning](https://azure.microsoft.com/en-us/services/machine-learning-service). See detailed instructions [here](https://docs.microsoft.com/en-us/azure/machine-learning/service/how-to-build-deploy-onnx).

# Installation
## APIs and Official Builds
| API Documentation | CPU package | GPU package |
|-----|-------------|-------------|
| [Python](https://aka.ms/onnxruntime-python) | [Windows/Linux/Mac](https://pypi.org/project/onnxruntime/)| [Windows/Linux](https://pypi.org/project/onnxruntime-gpu/)<br>(Compatible with CUDA9.1 and cuDNN 7.3) |
| [C#](docs/CSharp_API.md) | [Windows/Linux](https://www.nuget.org/packages/Microsoft.ML.OnnxRuntime/)<br>Mac - Coming Soon| [Windows/Linux](https://www.nuget.org/packages/Microsoft.ML.OnnxRuntime.Gpu/)<br>Mac - Coming Soon|
| [C](docs/C_API.md) | [Windows/Linux](https://www.nuget.org/packages/Microsoft.ML.OnnxRuntime/)<br>Mac - Coming Soon | [Windows/Linux](https://www.nuget.org/packages/Microsoft.ML.OnnxRuntime.Gpu/)<br>Mac - Coming Soon |
| [C++](onnxruntime/core/session/inference_session.h) | TBD | TBD |

## System Requirements
* ONNX Runtime binaries in CPU packages use OpenMP and depends on the library being available at runtime in the
system.
  * For Windows, OpenMP support comes as part of VC runtime. It is also available as redist packages:
    [vc_redist.x64.exe](https://aka.ms/vs/15/release/vc_redist.x64.exe) and [vc_redist.x86.exe](https://aka.ms/vs/15/release/vc_redist.x86.exe)
  * For Linux, the system must have the libgomp.so.1 which can be installed using ```apt-get install libgomp1```.
* The GPU builds require the CUDA9.1 and cuDNN 7.3 runtime libraries being installed in the system.

## Build Details
For details on the build configurations and information on how to create a build, see [Build ONNX Runtime](BUILD.md).

## Versioning
See more details on API and ABI Versioning and ONNX Compatibility in [Versioning](docs/Versioning.md).

# Design and Key Features
For an overview of the high level architecture and key decisions in the technical design of ONNX Runtime, see [Engineering Design](docs/HighLevelDesign.md).

ONNX Runtime is built with an extensible design that makes it versatile to support a wide array of models with high performance.

* [Add a custom operator/kernel](docs/AddingCustomOp.md)
* [Add an execution provider](docs/AddingExecutionProvider.md)
* [Add a new graph
transform](include/onnxruntime/core/optimizer/graph_transformer.h)
* [Add a new rewrite rule](include/onnxruntime/core/optimizer/rewrite_rule.h)

# Contribute
We welcome your contributions! Please see the [contribution guidelines](CONTRIBUTING.md).

## Feedback
For any feedback or to report a bug, please file a [GitHub Issue](https://github.com/Microsoft/onnxruntime/issues).

## Code of Conduct
This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/)
or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

# License
[MIT License](LICENSE)
