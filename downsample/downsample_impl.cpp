#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#include <iostream>
#include <cmath>

namespace downsample
{
	using namespace std;

	using namespace boost::python;
	namespace np = boost::python::numpy;

	void complain(const std::string message)
	{
		PyErr_SetString(PyExc_TypeError, message.c_str());
		throw_error_already_set();
	}

	template <class Real>
	void find_minmax(
		const Real * data,
		const int size,
		int & mini,
		Real & min,
		int & maxi,
		Real & max)
	{
		mini = 0;
		min = data[mini];
		maxi = 0;
		max = data[maxi];

		for (int i = 1; i < size; ++i)
		{
			auto value = data[i];
			if (value > max)
			{
				maxi = i;
				max = value;
			}
			else if (value < min)
			{
				mini = i;
				min = value;
			}
		}
	}

	template <class Real>
	void minmax_impl(
		const np::ndarray & input,
		np::ndarray & output,

		const int chunks,
		const int chunk_size)
	{
		const Real * input_data = reinterpret_cast<const Real *>(input.get_data());
		Real * output_data = reinterpret_cast<Real *>(output.get_data());
		int oi = 0;
		for (int ci = 0; ci < chunks; ++ci, oi += 2, input_data += chunk_size)
		{
			int mini, maxi;
			Real p0, p1;
			find_minmax<Real>(input_data, chunk_size, mini, p0, maxi, p1);
			if (maxi < mini)
			{
				output_data[oi] = p1;
				output_data[oi + 1] = p0;
			}
			else
			{
				output_data[oi] = p0;
				output_data[oi + 1] = p1;
			}
		}
	}

	// Designed per
	// http://zone.ni.com/reference/en-XX/help/371361H-01/lvconcepts/memory_management_for_large_data_sets/
	np::ndarray minmax(const np::ndarray & input, int bins)
	{
		if (input.get_nd() != 1)
			complain("Only vectors are supported.");
		if (input.get_dtype() != np::dtype::get_builtin<double>()
				and input.get_dtype() != np::dtype::get_builtin<float>())
			complain("Vector type should be double or float.");
		if (!(input.get_flags() & np::ndarray::C_CONTIGUOUS))
			complain("Vector data must be contiguous in memory.");
		if (bins < 10)
			complain("You should have more than ten bins.");
		auto N = input.shape(0);
		if (N <= 2*bins)
			return input;

		auto chunks_per_bin = 2.0;
		auto chunks = bins*chunks_per_bin;
		auto chunk_size = static_cast<int>(std::ceil(N/chunks));
		chunks = static_cast<int>(std::floor(N/chunk_size));
		auto size = static_cast<int>(chunks*2);

		np::ndarray output = np::empty(boost::python::make_tuple(size), input.get_dtype());
		if (input.get_dtype() == np::dtype::get_builtin<double>())
			minmax_impl<double>(input, output, chunks, chunk_size);
		else if (input.get_dtype() == np::dtype::get_builtin<float>())
			minmax_impl<float>(input, output, chunks, chunk_size);
		else
			complain("Something went wrong.");

		return output;
	}
}

BOOST_PYTHON_MODULE(downsample_impl)
{
	using namespace boost::python;
	namespace np = boost::python::numpy;

	np::initialize();

	def("minmax", downsample::minmax);
}
