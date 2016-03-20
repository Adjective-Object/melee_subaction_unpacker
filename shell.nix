let pkgs = import /home/adjective/Projects/nixpkgs {};
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
        
        ansifilter
    ];
    
    dependencies = [
    ];

in {
    devEnv = stdenv.mkDerivation {
        name = "mreader";
        buildInputs = devDependencies ++ dependencies;
    };
}

