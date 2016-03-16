let pkgs = import <nixpkgs> {};
in with pkgs; let
    
    devDependencies = [
        stdenv
        # build
        gcc
        gnumake
        cmake

        # check
        llvmPackages.clang-unwrapped # for clang-format
        cppcheck
        valgrind
        gdb

        # etc
        hexcurse
        ctags
    ];
    
    dependencies = [
       # bioviewer deps
       SDL
       SDL_image
    ];

in {
    devEnv = stdenv.mkDerivation {
        name = "mreader";
        buildInputs = devDependencies ++ dependencies;
    };
}

