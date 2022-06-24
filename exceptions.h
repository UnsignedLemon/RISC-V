#ifndef EXP_H
#define EXP_H
namespace riscv{
	
class exception{};

//---------------    R E A L    E X C E P T I O N S    -------------------------
class open_file_failed : public exception{};
class invalid_assembly_code : public exception{};
class invalid_data_fetch : public exception{};

}

#endif
