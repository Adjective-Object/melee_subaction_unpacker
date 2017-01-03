let pkgs = import <nixpkgs> {};
in with pkgs; let
    
    devDependencies = [
        stdenv
        # build
        gcc
        gnumake
        cmake
        binutils
        glibc

        # check
        llvmPackages.clang-unwrapped # for clang-format
        cppcheck
        valgrind
        gdb

        # etc
        hexcurse
        ctags
        
        ansifilter
        zsh
    ];
    
    dependencies = [
        unzip cmake boost zlib 
    ];

in {
    devEnv = stdenv.mkDerivation {
        name = "mreader";
        buildInputs = devDependencies ++ dependencies;
        shellHook = ''
        export CMAKE_CXX_COMPILER=${gcc}/bin/g++
        '';
    };
}

