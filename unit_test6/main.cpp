#include <fstream>
#include <utility>

#include "bobox_manager.hpp"
#include "bobox_results.hpp"
#include "bobox_bobolang.hpp"
#include "bobox_basic_object_factory.hpp"

#include "output.h"

class source_box : public bobox::basic_box {
public:
	typedef generic_model<source_box> model;

	source_box(const bobox::box_parameters_pack &box_params) : bobox::basic_box(box_params) {}

	BOBOX_BOX_INPUTS_LIST(poison_in, 0);
	BOBOX_BOX_OUTPUTS_LIST(data_out, 0);
private:
	virtual void sync_body() BOBOX_OVERRIDE
	{
		static const bobox::column_index_type data_out_0( 0);
#ifdef _DEBUG
		static const long long n = 1000LL;
#else
		static const long long n = 1000LL;
#endif

		for ( long long i = 0; i < n;)
		{
			auto out_env = allocate( outputs::data_out());
			auto size = unsigned( std::min( n - i, (long long)( out_env->get_capacity())));
			out_env->set_size( size);	

			auto out_data_0 = out_env->get_column( data_out_0).get_data< int>();

			for ( std::size_t j = 0; j < size; ++ j)
			{
				out_data_0[ j] = int( i + j);	
			}

			send_envelope( outputs::data_out(), out_env);
			i += size;
			wait();
		}

		send_poisoned( outputs::data_out());
	}
};

typedef output_box add_box;

/*
class add_box : public bobox::basic_box {
public:
	typedef generic_model<add_box> model;

	add_box(const bobox::box_parameters_pack &box_params) : bobox::basic_box(box_params) {}

	BOBOX_BOX_INPUTS_LIST(data_in_a, 0, data_in_b, 1);
	BOBOX_BOX_OUTPUTS_LIST(data_out, 0);
private:
	virtual void sync_body() BOBOX_OVERRIDE
	{
		static const bobox::column_index_type data_in_a_0( 0);
		static const bobox::column_index_type data_in_b_0( 0);
		static const bobox::column_index_type data_out_0( 0);

		unsigned pos_a = 0;
		unsigned pos_b = 0;
		unsigned pos_o = 0;

		unsigned size_a = 0;
		unsigned size_b = 0;
		unsigned capacity_o = 0;

		bobox::envelope_ptr_type in_a_env;
		bobox::envelope_ptr_type in_b_env;
		bobox::mutable_envelope_ptr_type out_env;

		const int * in_a_data_0;
		const int * in_b_data_0;
		int * out_data_0;

		for (;;)
		{
			if ( pos_a == size_a )
			{
				in_a_env.reset();
				in_a_env = pop_envelope( inputs::data_in_a());
				if ( in_a_env->is_poisoned() )
					break;
				size_a = in_a_env->get_size();
				pos_a = 0;
				in_a_data_0 = in_a_env->get_column( data_in_a_0).get_data< int>();
				continue;	// to skip empty envelopes
			}

			if ( pos_b == size_b )
			{
				in_b_env.reset();
				in_b_env = pop_envelope( inputs::data_in_b());
				if ( in_b_env->is_poisoned() )
					break;
				size_b = in_b_env->get_size();
				pos_b = 0;
				in_b_data_0 = in_b_env->get_column( data_in_b_0).get_data< int>();
				continue;	// to skip empty envelopes
			}

			if ( pos_o == capacity_o )
			{
				if ( pos_o )
				{
					out_env->set_size( pos_o);
					send_envelope( outputs::data_out(), out_env);
					wait();
				}

				out_env = allocate( outputs::data_out());
				capacity_o = out_env->get_capacity();
				pos_o = 0;
				out_data_0 = out_env->get_column( data_out_0).get_data< int>();
			}

			auto batch_size = std::min( std::min( size_a - pos_a, size_b - pos_b), capacity_o - pos_o);

			for ( std::size_t j = 0; j < batch_size; ++ j)
			{
				out_data_0[ pos_o + j] = in_a_data_0[ pos_a + j] + in_b_data_0[ pos_b + j];
			}

			pos_a += batch_size;
			pos_b += batch_size;
			pos_o += batch_size;
		}

		if ( pos_o )
		{
			out_env->set_size( pos_o);
			send_envelope( outputs::data_out(), out_env);
		}

		send_poisoned( outputs::data_out());
	}
};
*/

class sink_box : public bobox::basic_box {
public:
	typedef generic_model<sink_box> model;

	sink_box(const bobox::box_parameters_pack &box_params) : bobox::basic_box(box_params) {}

	BOBOX_BOX_INPUTS_LIST(data_in, 0);
	BOBOX_BOX_OUTPUTS_LIST(poison_out, 0);
private:
	virtual void sync_body() BOBOX_OVERRIDE
	{
		static const bobox::column_index_type data_in_0( 0);

		long long sum = 0;

		for (;;)
		{
			auto in_env = pop_envelope( inputs::data_in());

			if ( in_env->is_poisoned() )
				break;

			auto size = in_env->get_size();

			auto in_data_0 = in_env->get_column( data_in_0).get_data< int>();

			for ( std::size_t j = 0; j < size; ++ j)
			{
				sum += in_data_0[ j];
			}
		}

		suspend();
		std::cout << sum << std::endl;	// !!! illegal: I am still running on some strange thread
		resume();

		send_poisoned( outputs::poison_out());
	}
};

class runtime : public bobox::runtime, public bobox::basic_object_factory {
	virtual bobox::runtime *get_runtime() BOBOX_OVERRIDE { return this; }

private:
	virtual void init_impl() BOBOX_OVERRIDE 
	{
		register_type< int>( bobox::type_tid_type( "int"));

		register_box< source_box::model>( bobox::box_model_tid_type( "hello::source_box"));
		register_box< add_box::model>( bobox::box_model_tid_type( "hello::add_box"));
		register_box< sink_box::model>( bobox::box_model_tid_type( "hello::sink_box"));
	}
};

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " file [number of replicas]" << std::endl;
		return 1;
	}

	auto manager_params = new bobox::basic_parameters;
	if (argc == 3) {
		manager_params->add_parameter("OptimalPlevel", bobox::plevel_type(std::stoi(argv[2])));
	}
	bobox::manager mng((bobox::parameters_ptr_type(manager_params)));

	runtime rt;
	rt.init();

	std::string filename = argv[1];
	std::ifstream in(filename);
	if (!in) {
		std::cerr << "Failed to open file '" << filename << "'" << std::endl;
		return 1;
	}

	bobox::request_id_type rqid;
	try {
		rqid = mng.create_request(bobox::bobolang::compile(in, &rt));
	} catch (std::exception &exc) {
		std::cerr << "Compilation failed: " << exc.what() << std::endl;
		return 1;
	}

	mng.run_request(rqid);
	mng.wait_on_request(rqid);

	{
		std::ofstream dot(filename + ".dot");
		bobox::dot_printer printer(false, false);
		mng.dump_request(rqid, printer, dot);
	}

	{
		std::ofstream json(filename + ".json");
		bobox::json_printer printer;
		mng.dump_request(rqid, printer, json);
	}

	std::cout << "Request result code: " << mng.get_result(rqid) << std::endl;

	mng.destroy_request(rqid);

	return 0;
}
