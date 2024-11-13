// Import the functions you need from the SDKs you need

import { initializeApp } from "firebase/app";
import "firebase/database";
import { get, getDatabase, onValue, ref, set } from "firebase/database";

// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyBcsPB1BHwtarNSaYdSH6Pghaa5he0A6wc",
  authDomain: "iotproject-5c153.firebaseapp.com",
  databaseURL: "https://iotproject-5c153-default-rtdb.firebaseio.com",
  projectId: "iotproject-5c153",
  storageBucket: "iotproject-5c153.appspot.com",
  messagingSenderId: "377219488662",
  appId: "1:377219488662:web:468a2e207d99f36ef29bb7",
  measurementId: "G-8GJQ0B44R9"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

const db = getDatabase(app);

export { db, get, onValue, ref, set };

