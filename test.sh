echo "=========================================="
echo "   TESTS DE VALIDATION DU MINI-SGF"
echo "=========================================="

rm -f disk.img

echo ""
echo ">>> Compilation du projet..."
make > /dev/null 2>&1
if [ ! -f sgf ]; then
    echo "ECHEC : la compilation a echoue."
    exit 1
fi
echo "Compilation OK."

echo ""
echo ">>> SESSION 1 : creation de fichiers et dossiers"
echo "--------------------------------------------------"
./sgf << 'FIN'
mkdir documents
mkdir images
echo bonjour > salut.txt
echo test123 > documents
ls
df
exit
FIN

echo ""
echo ">>> SESSION 2 : verification de la PERSISTANCE"
echo "--------------------------------------------------"
echo "(On relance le shell : les donnees doivent etre la)"
./sgf << 'FIN'
ls
cat salut.txt
exit
FIN

echo ""
echo "=========================================="
echo "   FIN DES TESTS"
echo "=========================================="
echo "Verifiez ci-dessus que :"
echo " - SESSION 1 a cree documents/, images/, salut.txt"
echo " - SESSION 2 retrouve ces elements (persistance OK)"
echo " - 'cat salut.txt' affiche bien 'bonjour'"