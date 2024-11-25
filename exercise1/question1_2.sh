
give_answer(){
    if [ "$1" == "Y" ]; then
        echo "Give me the values using the format you should"
        read values
         if [[ "$values" =~ ^[0-9]+,[^0-9,]+,[0-9]+,[^0-9,]+,(Passenger|Crew),(Yes|No)$ ]]; then
            echo "Values are correct"
            echo "$values" >> "$working_file"
            echo "Wanna insert new line?(Y/n)"
            read answer
            give_answer $answer
        else
            echo "Values are not correct"
            exit 1
        fi
    fi
}

fetch_csv_file(){
    echo "Give me the file path (please the path of the csv file)"
    read file_dir
    echo "working directory:$(pwd)"
    if [[ $file_dir != *.csv ]]; then
        echo "The provided file is not a .csv file or you choosed to insert values manually"
        echo "You should insert values using keyboard?(Y/n)"
        read answer
        if [ "$answer" == "n" ]; then
            exit 1
        fi
        working_file="$(pwd)/passengers.csv"
        echo "code,fullname,age,country,status,rescued" >> "$working_file"
        give_answer $answer
        exit 1
    else
        echo "The provided file is a .csv file"
        working_file=$file_dir
        if [ -f $working_file ]; then
            echo "The file exists and your data is in our safe hands"
        else
            echo "However, The file does not exist"
            exit 1
        fi
    fi
}
search_by_name(){
    echo "Enter the name or surname to search"
    read name
    # the name should be between commas
    if [[ "$name" =~ ^[^,]+( [^,]+)*$ ]]; then
        echo "Search term is correct"
        grep -iE ",[^,]*$name[^,]*," "$working_file" # finding records in the csv file
    else
        echo "Search term is not correct"
        exit 1
    fi
}


fetch_csv_file
search_by_name


