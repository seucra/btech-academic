use std::io;

fn fizzbuzz() {
    for i in 1..=100 {
        match i {
            n if n % 15 ==0 => println!("FizzBuzz"),
            n if n % 3  ==0 => println!("Fizz"),
            n if n % 5  ==0 => println!("Buzz"),
            _               => println!("{}", i),
        }
    }
}

fn palindrome(s: &str) {
    let cleaned: String = s
        .chars()
        .filter(|c| c.is_alphanumeric())
        .map(|c| c.to_ascii_lowercase())
        .collect();

    let res = if s.chars().eq(s.chars().rev()) {"palindrome"} else {"not palindrome"};
    println!("{} is {}", s, res);
}

fn main () {
    // fizzbuzz();
    palindrome("racecar");
}
