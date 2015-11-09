#/usr/bin/env python
from bs4 import BeautifulSoup
from sys import stdin

def main():
    doc = stdin.read()
    soup= BeautifulSoup(doc, "html.parser")
    [s.extract() for s in soup('style')]
    [s.extract() for s in soup('link')]

    soup.head.append(soup.new_tag("link",
        rel="stylesheet",
        href="style.css"))

    soup.body.insert(1,
        BeautifulSoup("""<nav><a href="./index.html">&#x25c0;</a></nav>"""))

    print soup

if __name__ == "__main__":
    main()
