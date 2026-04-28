{ pkgs? import ‹nixpkgs› {} }:

pkgs.mkShell {
    packages = [
        (pkgs.python3.withPackages (python-pkgs: [
            python-pkgs.matplotlib
        ]))
        pkgs.scons
        pkgs.gmt
    ];
} 
