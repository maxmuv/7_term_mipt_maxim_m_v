#include <string.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

static int num_functions = 0;
static int func_in_list = 100;

struct Func {
  Func(std::ofstream &stream, int f, int h, bool set_timer, std::string &name) {
    if (0 == h) {
      name = "main_" + std::to_string(reinterpret_cast<long uint>(this)) + "_" +
             "set_timer" + "_" + std::to_string(set_timer) + "()";
      std::vector<std::unique_ptr<Func>> childs;
      int number_of_childs =
          (0 != f % func_in_list) ? f / func_in_list + 1 : f / func_in_list;
      child_names.resize(number_of_childs);
      for (int i = 0; i < f / func_in_list; i++) {
        childs.emplace_back(
            new Func(stream, func_in_list, 1, set_timer, child_names[i]));
      }
      if (f % func_in_list)
        childs.emplace_back(new Func(stream, f % func_in_list, 1, set_timer,
                                     child_names[number_of_childs - 1]));
      stream << "\n";
      stream << "void " << name << "{" << std::endl;
      stream << "SetTimer;\n";
      for (int i = 0; i < number_of_childs; i++) {
        stream << child_names[i] << ";" << std::endl;
      }
      stream << "}" << std::endl;
    } else if (2 == h) {
      num_functions++;
      name = "f_" + std::to_string(reinterpret_cast<long uint>(this)) +
             "set_timer_" + std::to_string(set_timer) + "_id_" +
             std::to_string(num_functions) + "()";
      stream << "\n";
      stream << "void " << name << "{" << std::endl;
      if (set_timer) stream << "SetTimer;\n";
      stream << "int j = 1;\n";
      stream << "std::vector<int> v;\n";
      stream << "std::vector<int> z;\n";
      stream << "for (int i = 0; i < 1000000; i++){\n";
      stream << "for (int n = 0; n < 100; n++)\n";
      stream << "if (j > 0) j--;\n";
      stream << "else j++;\n";
      stream << "v.push_back(j);}\n";
      stream << "z.push_back(1 - j);\n";
      stream << "}" << std::endl;
    } else {
      num_functions++;
      name = "f_" + std::to_string(num_functions) + "()";
      std::vector<std::unique_ptr<Func>> childs;
      child_names.resize(f);
      for (int i = 0; i < f; i++) {
        childs.emplace_back(new Func(stream, 0, 2, set_timer, child_names[i]));
      }
      stream << "void " << name << "{\n";
      for (int i = 0; i < f; i++) {
        stream << child_names[i] << ";" << std::endl;
      }
      stream << "}\n";
    }
  }
  std::vector<std::string> child_names;
};

class Generator {
 public:
  Generator() = delete;
  Generator(int argc, char **argv);
  ~Generator() {}

 public:
  void CreateTests() {
    std::ofstream path_2_file(path);
    path_2_file << "#pragma once\n";
    path_2_file << "#include \"../../new_timer_impl.h\"\n";
    path_2_file << "#include \"vector\"\n";
    std::string name1, name2;
    std::unique_ptr<Func> f1(new Func(path_2_file, functions, 0, true, name1));
    std::unique_ptr<Func> f2(new Func(path_2_file, functions, 0, false, name2));
    path_2_file << std::endl;
    path_2_file << "void  "
                << "f_" << std::to_string(functions) << "(){" << std::endl;
    path_2_file << name1 << ";" << std::endl;
    path_2_file << name2 << ";" << std::endl;
    path_2_file << "}";
  }

 private:
  std::string path;
  int functions;
};

int main(int argc, char **argv) {
  try {
    Generator gen(argc, argv);
    gen.CreateTests();
    std::cout << "generate " + std::to_string(num_functions) + " functions";
  } catch (std::string &ex) {
    std::cout << ex;
  }
}

Generator::Generator(int argc, char **argv) {
  path = "";
  functions = 0;
  for (int i = 1; i < argc; i += 2) {
    if (std::string(argv[i]) == "-p") {
      path = std::string(argv[i + 1]);
    }
    if (std::string(argv[i]) == "-f") {
      functions = std::stoi(argv[i + 1]);
    }
  }
  if ((path == "") && (!functions))
    throw std::string("Error in generator's constructor");
}
