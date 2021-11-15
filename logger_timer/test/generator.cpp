#include <string.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

struct Func {
  Func(Func *p, std::ofstream &stream, int h, int w, bool set_timer,
       std::string &name) {
    if (!p) {
      name = "main_" + std::to_string(reinterpret_cast<long uint>(this)) + "_" +
             "set_timer" + "_" + std::to_string(set_timer) + "()";
      std::vector<std::unique_ptr<Func>> childs;
      child_names.resize(w);
      for (int i = 0; i < w; i++) {
        childs.emplace_back(
            new Func(this, stream, h - 1, w - 1, set_timer, child_names[i]));
      }
      stream << "\n";
      stream << "void " << name << "{" << std::endl;
      stream << "SetTimer;\n";
      for (int i = 0; i < w; i++) {
        stream << child_names[i] << ";" << std::endl;
      }
      stream << "}" << std::endl;
    } else if (h = 1) {
      name = "f_" + std::to_string(reinterpret_cast<long uint>(this)) + "_" +
             std::to_string(h) + "_" + "set_timer_" +
             std::to_string(set_timer) + "()";
      stream << "\n";
      stream << "void " << name << "{" << std::endl;
      if (set_timer) stream << "SetTimer;\n";
      stream << "for (int i = 0; i < 10; i++);\n";
      stream << "}" << std::endl;
    } else {
      name = "f_" + std::to_string(reinterpret_cast<long uint>(this)) + "_" +
             std::to_string(h) + "_" + "set_timer_" +
             std::to_string(set_timer) + "()";
      std::vector<std::unique_ptr<Func>> childs;
      child_names.resize(w);
      for (int i = 0; i < w; i++) {
        childs.emplace_back(
            new Func(this, stream, h - 1, w - 1, set_timer, child_names[i]));
        stream << "\n";
        stream << "void " << name << "{" << std::endl;
        if (set_timer) stream << "SetTimer;\n";
        for (int i = 0; i < w; i++) {
          stream << child_names[i] << ";" << std::endl;
        }
        stream << "}" << std::endl;
      }
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
    path_2_file << "#include \"new_timer_impl.h\"\n";
    std::string name1, name2;
    std::unique_ptr<Func> f1(
        new Func(nullptr, path_2_file, height, width, true, name1));
    std::unique_ptr<Func> f2(
        new Func(nullptr, path_2_file, height, width, false, name2));
    path_2_file << std::endl;
    path_2_file << "#define "
                << "f_h" << std::to_string(height) << "_w_"
                << std::to_string(width) << std::endl;
    path_2_file << name1 << ";\\" << std::endl;
    path_2_file << name2 << ";\\" << std::endl;
  }

 private:
  std::string path;
  int height;
  int width;
};

int main(int argc, char **argv) {
  try {
    Generator gen(argc, argv);
    gen.CreateTests();
  } catch (std::string &ex) {
    std::cout << ex;
  }
}

Generator::Generator(int argc, char **argv) {
  path = "";
  height = 0;
  width = 0;
  for (int i = 1; i < argc; i += 2) {
    if (std::string(argv[i]) == "-p") {
      path = std::string(argv[i + 1]);
    }
    if (std::string(argv[i]) == "-h") {
      height = std::stoi(argv[i + 1]);
    }
    if (std::string(argv[i]) == "-w") {
      width = std::stoi(argv[i + 1]);
    }
  }
  if ((path == "") && (!height) && (!width))
    throw std::string("Error in generator's constructor");
}
