if [ -e $1.hpp ]; then
	echo "Datei existiert bereist!"
	echo "--force zum überschreiben"
	if [ "$2" = "--force" ]; then
		echo "wird überschrieben"
	else
		exit
	fi
fi

if [ $1 ]; then
	bla=`echo $1 | tr "[:lower:]" "[:upper:]"`
	echo -e "#ifndef "$bla"_HPP_INCLUDED\n#define "$bla"_HPP_INCLUDED" > $1.hpp
	echo -e "\n//------------------------------------------------------------------------------------------------" >> $1.hpp
	echo -e "\n//------------------------------------------------------------------------------------------------" >> $1.hpp
	echo -e "\n#endif //"$bla"_HPP_INCLUDED" >> $1.hpp

	echo -e "#include \""$1".hpp\"\n" > $1.cpp
	echo    "//------------------------------------------------------------------------------------------------" >> $1.cpp
	echo    "//------------------------------------------------------------------------------------------------" >> $1.cpp
	echo    "//------------------------------------------------------------------------------------------------" >> $1.cpp
else
	echo "Bitte Name angeben."
fi
