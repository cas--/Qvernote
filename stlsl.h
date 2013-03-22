// stlsl.h : Declares and Defines the STL Serialization Libray (STL-SL) template classes.
// Author: Aniruddha Jayant Jahagirdar (ajDOTsubscribeAT-THE-RATEgmailFULL-STOPcom)
// Date: October 2, 2006
// Revision: 0.1

//required for STL-SL classes
#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <stack>
#include <queue>
#include <deque>

//required for 'stl_trait_writer' and 'file_trait_reader'
#include <fstream>


//Serialization-file writer class.
//This guy writes the data to the file specified by 'file_path'.
//NOTE: This class does not recoginse the data objects containing spaces, tabs, newline 
// charactes in them. This may be fixed by overloading '<<' operator and adding
// escape-sequencing logic in it.
class stl_trait_writer: public std::ofstream
{
public:

	stl_trait_writer(const std::string& file_path):std::ofstream(file_path.c_str())
	{

	}
};

//Serialization-file reader class.
//This guy reads the data from the file specified by 'file_path'.
//NOTE: This class does not recoginse the data objects containing spaces, tabs, newline 
// charactes in them. This may be fixed by overloading '>>' operator and adding
// escape-sequencing logic in it.
class file_trait_reader: public std::ifstream
{
public:

	file_trait_reader(const std::string& file_path):std::ifstream(file_path.c_str())
	{

	}
};

//Serialization filer class.
//This guy presents the set of reader, writer objects responsible for reading and writing 
// to the serialization file.
template <class writer_trait , class reader_trait>
class filer_trait
{
public:

	typedef typename writer_trait writer_type;
	typedef typename reader_trait reader_type;
};

//Basic datatype serializer class.
//Triggers the read or write to the serialization file for the basic datatypes.
//NOTE: This class has been tweaked to work with the 'stl_trait_writer' class.
template <class val_trait, class val_filer_trait = filer_trait<stl_trait_writer, file_trait_reader> >
class value_trait
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	void serialize(const val_trait& val, writer_trait &pen)
	{
		pen << val << "\n"; //a tweak for 'stl_trait_writer' class defined above.
		//pen << val;		//correct code, this should replace above line of code, should you impliment your own 'stl_trait_writer' class
		pen.flush();
	}

	void load(val_trait& val, reader_trait &pen)
	{
		pen >> val;
	}
};

//Associative-list datatype serializer class.
//Triggers the read or write to the serialization file for the Associative-list datatypes.
//This class takes care of STL types -- map, multimap, set, multiset
template <class associative_list_type, class val_filer_trait >
class associative_list_value_trait
{
public:
	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef typename associative_list_type::size_type size_type;
	typedef typename associative_list_type::key_type key_type;
	typedef typename associative_list_type::mapped_type data_type;

	void serialize (associative_list_type& val, writer_trait &pen )
	{
		value_trait<size_type, val_filer_trait> size_filer;
		size_filer.serialize (val.size(), pen);

		for(associative_list_type::iterator i=val.begin(); i != val.end(); i++)
		{
			value_trait<key_type, val_filer_trait> val_trait_key_filer;
			value_trait<data_type, val_filer_trait> val_trait_data_filer;

			val_trait_key_filer.serialize(i->first,pen);
			val_trait_data_filer.serialize(i->second,pen);
		}
	}

	void load (associative_list_type& val, reader_trait &pen )
	{
		value_trait<size_type, val_filer_trait> size_reader;
		size_type val_size=0;
		size_reader.load(val_size, pen);

		for(; val_size > 0; val_size--)
		{
			key_type key_element;
			value_trait<key_type, val_filer_trait> val_trait_key_reader;
			val_trait_key_reader.load(key_element, pen);

			data_type data_element;
			value_trait<data_type, val_filer_trait> val_trait_data_reader;
			val_trait_data_reader.load(data_element, pen);

			val.insert (std::pair<key_type, data_type> (key_element, data_element));
		}

	}
};

//Sequence-list datatype serializer class.
//Triggers the read or write to the serialization file for the Sequence-list datatypes.
//This class takes care of STL types -- list, vector, stack, queue, deque, priority_queue
//NOTE: 'basic_string' type is not treated as sequence-list in this implimentation.
template <class sequence_list_type, class val_filer_trait >
class sequence_list_value_trait
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef typename sequence_list_type::size_type size_type;	
	typedef typename sequence_list_type::value_type value_type;

	void serialize (sequence_list_type& val, writer_trait &pen )
	{
		value_trait<size_type, val_filer_trait> size_filer;
		size_filer.serialize (val.size(), pen);

		for(sequence_list_type::iterator i=val.begin(); i != val.end(); i++)
		{
			value_trait<value_type, val_filer_trait> val_trait_key_filer;
			val_trait_key_filer.serialize(*i,pen);

			//investigate: what goes in here in STL implimentation of sequence lists --
			//    from what I feel, it is the "next node pointer" of the interal linked-list
			//value_trait<val_trait_data, writer_trait> val_trait_data_filer;
		}
	}

	void load (sequence_list_type& val, reader_trait &pen )
	{
		value_trait<size_type, val_filer_trait> size_reader;
		size_type val_size=0;
		size_reader.load(val_size, pen);

		for(; val_size > 0; val_size--)
		{
			value_type element;
			value_trait<value_type, val_filer_trait> val_trait_key_reader;
			val_trait_key_reader.load(element, pen);
			val.push_back(element);
		}
	}
};

//STL map datatype serializer class.
//Triggers the read or write to the serialization file for the STL map datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::map<val_trait_key, val_trait_data> , val_filer_trait  > 
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::map<val_trait_key, val_trait_data> map_value_trait;

	void serialize (map_value_trait& val, writer_trait &pen )
	{

		associative_list_value_trait<map_value_trait, val_filer_trait> associative_list_value_filer;

		associative_list_value_filer.serialize (val, pen);
	}

	void load (map_value_trait& val, reader_trait &pen )
	{

		associative_list_value_trait<map_value_trait, val_filer_trait> associative_list_value_reader;

		associative_list_value_reader.load (val, pen);
	}
};


//STL multimap datatype serializer class.
//Triggers the read or write to the serialization file for the STL multimap datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::multimap<val_trait_key, val_trait_data> , val_filer_trait  > 
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::multimap<val_trait_key, val_trait_data> multimap_value_trait;

	void serialize (multimap_value_trait& val, writer_trait &pen )
	{

		associative_list_value_trait<multimap_value_trait, val_filer_trait> associative_list_value_filer;

		associative_list_value_filer.serialize (val, pen);
	}

	void load (multimap_value_trait& val, reader_trait &pen )
	{

		associative_list_value_trait<multimap_value_trait, val_filer_trait> associative_list_value_reader;

		associative_list_value_reader.load (val, pen);
	}
};

//STL set datatype serializer class.
//Triggers the read or write to the serialization file for the STL set datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::set<val_trait_key, val_trait_data> , val_filer_trait  > 
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::set<val_trait_key, val_trait_data> set_value_trait;

	void serialize (set_value_trait& val, writer_trait &pen )
	{

		associative_list_value_trait<set_value_trait, val_filer_trait> associative_list_value_filer;

		associative_list_value_filer.serialize (val, pen);
	}

	void load (set_value_trait& val, reader_trait &pen )
	{

		associative_list_value_trait<set_value_trait, val_filer_trait> associative_list_value_reader;

		associative_list_value_reader.load (val, pen);
	}
};

//STL multiset datatype serializer class.
//Triggers the read or write to the serialization file for the STL multiset datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::multiset<val_trait_key, val_trait_data> , val_filer_trait  > 
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::multiset<val_trait_key, val_trait_data> multiset_value_trait;

	void serialize (multiset_value_trait& val, writer_trait &pen )
	{

		associative_list_value_trait<multiset_value_trait, val_filer_trait> associative_list_value_filer;

		associative_list_value_filer.serialize (val, pen);
	}

	void load (multiset_value_trait& val, reader_trait &pen )
	{

		associative_list_value_trait<multiset_value_trait, val_filer_trait> associative_list_value_reader;

		associative_list_value_reader.load (val, pen);
	}
};


//STL list datatype serializer class.
//Triggers the read or write to the serialization file for the STL list datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::list<val_trait_key, val_trait_data> , val_filer_trait  >
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::list<val_trait_key, val_trait_data> list_value_trait;

	void serialize (list_value_trait& val, writer_trait &pen )
	{

		sequence_list_value_trait<list_value_trait, val_filer_trait> sequence_list_value_filer;

		sequence_list_value_filer.serialize (val, pen);
	}

	void load (list_value_trait& val, reader_trait &pen )
	{

		sequence_list_value_trait<list_value_trait, val_filer_trait> sequence_list_value_reader;

		sequence_list_value_reader.load (val, pen);
	}
};

//STL vector datatype serializer class.
//Triggers the read or write to the serialization file for the STL vector datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::vector<val_trait_key, val_trait_data> , val_filer_trait  >
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::vector<val_trait_key, val_trait_data> vector_value_trait;

	void serialize (vector_value_trait& val, writer_trait &pen )
	{

		sequence_list_value_trait<vector_value_trait, val_filer_trait> sequence_list_value_filer;

		sequence_list_value_filer.serialize (val, pen);
	}

	void load (vector_value_trait& val, reader_trait &pen )
	{

		sequence_list_value_trait<vector_value_trait, val_filer_trait> sequence_list_value_reader;

		sequence_list_value_reader.load (val, pen);
	}
};

//STL stack datatype serializer class.
//Triggers the read or write to the serialization file for the STL stack datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::stack<val_trait_key, val_trait_data> , val_filer_trait  >
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::stack<val_trait_key, val_trait_data> stack_value_trait;

	void serialize (stack_value_trait& val, writer_trait &pen )
	{

		sequence_list_value_trait<stack_value_trait, val_filer_trait> sequence_list_value_filer;

		sequence_list_value_filer.serialize (val, pen);
	}

	void load (stack_value_trait& val, reader_trait &pen )
	{

		sequence_list_value_trait<stack_value_trait, val_filer_trait> sequence_list_value_reader;

		sequence_list_value_reader.load (val, pen);
	}
};

//STL queue datatype serializer class.
//Triggers the read or write to the serialization file for the STL queue datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::queue<val_trait_key, val_trait_data> , val_filer_trait  >
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::queue<val_trait_key, val_trait_data> queue_value_trait;

	void serialize (queue_value_trait& val, writer_trait &pen )
	{

		sequence_list_value_trait<queue_value_trait, val_filer_trait> sequence_list_value_filer;

		sequence_list_value_filer.serialize (val, pen);
	}

	void load (queue_value_trait& val, reader_trait &pen )
	{

		sequence_list_value_trait<queue_value_trait, val_filer_trait> sequence_list_value_reader;

		sequence_list_value_reader.load (val, pen);
	}
};

//STL deque datatype serializer class.
//Triggers the read or write to the serialization file for the STL deque datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::deque<val_trait_key, val_trait_data> , val_filer_trait  >
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::deque<val_trait_key, val_trait_data> deque_value_trait;

	void serialize (deque_value_trait& val, writer_trait &pen )
	{

		sequence_list_value_trait<deque_value_trait, val_filer_trait> sequence_list_value_filer;

		sequence_list_value_filer.serialize (val, pen);
	}

	void load (deque_value_trait& val, reader_trait &pen )
	{

		sequence_list_value_trait<deque_value_trait, val_filer_trait> sequence_list_value_reader;

		sequence_list_value_reader.load (val, pen);
	}
};

//STL priority_queue datatype serializer class.
//Triggers the read or write to the serialization file for the STL priority_queue datatype .
template <class val_trait_key, class val_trait_data, class val_filer_trait  > 
class value_trait< std::priority_queue<val_trait_key, val_trait_data> , val_filer_trait  >
{
public:

	typedef typename val_filer_trait::writer_type writer_trait;
	typedef typename val_filer_trait::reader_type reader_trait;

	typedef std::priority_queue<val_trait_key, val_trait_data> priority_queue_value_trait;

	void serialize (priority_queue_value_trait& val, writer_trait &pen )
	{

		sequence_list_value_trait<priority_queue_value_trait, val_filer_trait> sequence_list_value_filer;

		sequence_list_value_filer.serialize (val, pen);
	}

	void load (priority_queue_value_trait& val, reader_trait &pen )
	{

		sequence_list_value_trait<priority_queue_value_trait, val_filer_trait> sequence_list_value_reader;

		sequence_list_value_reader.load (val, pen);
	}
};

