#![windows_subsystem = "windows"]

use std::process::Command;

fn main() {
    Command::new("cmd")
        .args(&["/C", "start", "https://www.youtube.com/watch?v=dQw4w9WgXcQ"])
        .spawn()
        .expect("Не удалось открыть браузер");
}
