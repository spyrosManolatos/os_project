#!/bin/bash

# Function to fetch the CSV file
fetch_csv_file() {
    echo "Give me the file path (please the path of the csv file)"
    read file_dir
    echo "Working directory: $(pwd)"
    if [[ "$file_dir" != *.csv ]]; then
        echo "The provided file is not a .csv file or you chose to insert values manually"
        echo "You should insert values using keyboard? (Y/n)"
        read answer
        if [[ "$answer" == "n" ]]; then
            exit 1
        fi
        working_file="$(pwd)/passengers.csv"
    else
        echo "The provided file is a .csv file"
        working_file=$file_dir
        if [[ -f "$working_file" ]]; then
            echo "The file exists and your data is in our safe hands"
        else
            echo "However, the file does not exist"
            exit 1
        fi
    fi
}

# Function to change delimiter from semicolon to comma
change_delimiter() {
    sed 's/;/,/g' "$working_file" > temp.csv && mv temp.csv "$working_file"
}
# Function to update a record by name
update_record_by_name() {
    full_name="$1"
    new_record="$2"
    awk -F, -v full_name="$full_name" -v new_record="$new_record" '
    BEGIN {OFS=FS}
    $2 == full_name {
        print "Original line: " $0 > "/dev/stderr"
        $0 = new_record
    }
    {print}
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
    echo "Record updated by name"
}
# Function to update a record by code
update_record_by_code() {
    code="$1"
    new_record="$2"
    awk -F, -v code="$code" -v new_record="$new_record" '
    BEGIN {OFS=FS}
    $1 == code {
        print "Original line: " $0 > "/dev/stderr"
        $0 = new_record
    }
    {print}
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
    echo "Record updated by code"
}

add_new_record() {
    new_record="$1"
    if [[ "$new_record" =~ ^[0-9]+,[^,]+,[^,]+,[^,]+$ ]]; then
        echo "$new_record" >> "$working_file"
        echo "New record added"
    else
        echo "New record format is incorrect"
        exit 1
    fi
}
# Function to update full name by code and display the original line
update_full_name_by_code() {
    code="$1"
    new_full_name="$2"
    awk -F, -v code="$code" -v full_name="$new_full_name" '
    BEGIN {OFS=FS}
    $1 == code {
        print "Original line: " $0 > "/dev/stderr"
        $2 = full_name
    }
    {print} 
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
    echo "Full name updated by code"
}

# Function to update full name by full name and display the original line
update_full_name_by_full_name() {
    full_name="$1"
    new_full_name="$2"
    awk -F, -v full_name="$full_name" -v new_full_name="$new_full_name" '
    BEGIN {OFS=FS}
    $2 == full_name {
        print "Original line: " $0 > "/dev/stderr"
        $2 = new_full_name
    }
    {print}
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
    echo "Full name updated by full name"
}

# Function to take user input and determine the update method for records
# Function to update only the first name or surname
update_name() {
    search_term="$1"
    new_name="$2"
    awk -F, -v search_term="$search_term" -v new_name="$new_name" '
    BEGIN {OFS=FS}
    $2 ~ search_term {
        print "Original line: " $0 > "/dev/stderr"
        split($2, names, " ")
        if (names[1] == search_term) {
            names[1] = new_name
        } else if (names[2] == search_term) {
            names[2] = new_name
        }
        $2 = names[1] " " names[2]
    }
    {print}
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
    echo "Name updated"
}

update_record_by_name() {
    search_term="$1"
    new_record="$2"
    awk -F, -v search_term="$search_term" -v new_record="$new_record" '
    BEGIN {OFS=FS}
    $2 ~ search_term && !found {
        print "Original line: " $0 > "/dev/stderr"
        $0 = new_record
        found = 1
    }
    {print} 
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
    echo "Record updated"
}
take_user_input() {
    echo "Enter the search term (code, full name, or part of the name):"
    read search_term
    echo "Enter the new value (new full name, new record, or new name):"
    read new_value

    if [[ "$search_term" =~ ^[0-9]+$ ]] && [[ "$new_value" =~ ^[0-9]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+$ ]]; then
        echo "update_record_by_code"
        update_record_by_code "$search_term" "$new_value"
    elif [[ "$search_term" =~ ^[0-9]+$ ]]; then
        echo "update_full_name_by_code"
        update_full_name_by_code "$search_term" "$new_value"
    elif [[ "$search_term" =~ ^[^,]+$ ]] && [[ "$new_value" =~ ^[0-9]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+$ ]]; then
        echo "$search_term"
        echo "$new_value"
        if [[ "$search_term" =~ ^[0-9]+$ ]]; then
            echo "search term or new value should not be a number";
            exit;
        fi
        echo "update_record_by_name"
        update_record_by_name "$search_term" "$new_value"

    # elif [[ "$search_term" =~ ^[^,]+(\ [^,]+)*$ ]]; then
    #     echo "update_full_name_by_full_name"
    #     update_full_name_by_full_name "$search_term" "$new_value"
    elif [[ "$search_term" =~ ^[^,]+$ ]]; then
        if [[ "$search_term" =~ ^[0-9]+$ || "$new_value" =~ ^[0-9]+ ]]; then
            echo "search term or new value should not be a number";
            exit;
        fi
        echo "update_name"
        # update_name "$search_term" "$new_value"
    else
        echo "Search term is not correct"
        exit 1
    fi
}

# Main script execution
fetch_csv_file
change_delimiter
take_user_input
