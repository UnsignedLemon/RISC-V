#ifndef EXP_H
#define EXP_H
namespace riscv{
	
class exception{};

//---------------    R E A L    E X C E P T I O N S    -------------------------
class open_file_failed : public exception{};
class invalid_assembly_code : public exception{};
class invalid_data_fetch : public exception{};
class queue_empty : public exception{};
class unknown_not_match : public exception{};
class forwarding_not_match : public exception{};

}

#endif
