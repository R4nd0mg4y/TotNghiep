import { Account, Client } from "appwrite";
import express from "express";
import nodemailer from "nodemailer";
import { db, get, onValue, ref, set } from "./firebase.js";
// Khởi tạo ứng dụng express
const app = express();
const PORT = 3000; // Sử dụng cổng số cho server, ví dụ: 3000

// Middleware để parse JSON request body
app.use(express.json());

// Cấu hình Appwrite
const config = {
  endpoint: "https://cloud.appwrite.io/v1",
  projectId: "66f7db390034a1c09453",
};

// Khởi tạo Appwrite Client
const client = new Client()
  .setEndpoint(config.endpoint) // Your Appwrite Endpoint
  .setProject(config.projectId); // Your project ID

const account = new Account(client);

// Tạo transporter cho Gmail
const transporter = nodemailer.createTransport({
  host: "smtp.gmail.com",
  port: 465,
  secure: true,
  auth: {
    user: "tuandoviet2002@gmail.com", // Địa chỉ email của bạn
    pass: "pwld dctl jbsk ymif", // Mật khẩu ứng dụng của bạn
  },
});
app.post("/send-otp", async (req, res) => {
  const { email, otpCode } = req.body;


  console.log("Email received:", email); 

 
  if (!email || !email.includes("@")) {
    return res.status(400).send({ error: "Valid email is required" });
  }

  const mailOptions = {
    from: "tuandoviet2002@gmail.com",
    to: email,
    subject: "Your OTP Code",
    text: `Your OTP code is ${otpCode}`,
  };

  try {
    await transporter.sendMail(mailOptions);
    console.log("OTP sent to: ", email);
    res.status(200).send({ message: "OTP sent successfully", otpCode });
  } catch (error) {
    console.error("Error sending OTP: ", error);
    res.status(500).send({ error: "Failed to send OTP" });
  }
});

// Lưu trữ tạm thời userId và secret
// API để nhận userId và secret
let tempData = {}; // Khởi tạo biến toàn cục tạm thời

// Route nhận userId và secret từ Appwrite qua query
app.get("/reset-password", (req, res) => {
  const { userId, secret } = req.query;
  // console.log("Received data:", req.query);

  if (!userId || !secret) {
    return res.status(400).send("Invalid or missing parameters.");
  }

  // Lưu trữ userId và secret vào tempData
  tempData = { userId, secret };

  // Trả về thông báo thành công
  res.json({
    message: "Vui lòng nhập mật khẩu mới trên ứng dụng điện thoại.",
  });
});

// Route xử lý yêu cầu đổi mật khẩu
app.post("/change-password", async (req, res) => {
  const { newPassword } = req.body;
  const { userId, secret } = tempData;

  if (!userId || !secret || !newPassword) {
    return res.status(400).send("Missing parameters.");
  }

  try {
    // Cập nhật mật khẩu bằng Appwrite API
    await account.updateRecovery(userId, secret, newPassword);

    // Xóa dữ liệu tạm sau khi sử dụng
    tempData = {};
    res.status(200).send("Password reset successful!");
  } catch (error) {
    console.error("Error updating password:", error);
    res.status(500).send("Failed to update password.");
  }
});

app.post("/updateTime/:userId", async (req, res) => {
  const userId = req.params.userId;
  const { email } = req.body;
  console.log(email,userId);

  const currentTotalTimeRef = ref(db, `/users/${userId}/currentTotalTime`);
  const dataRef = ref(db,`devices/${email.replace(/\./g, "").toLowerCase()}/acceleration`);
  const postureErrorTimeRef = ref(db,`/users/${userId}/currentErrorPostureTime`);
  const timerRef = ref(db, `/users/${userId}/timer`);
  const connectStatusRef = ref(db, `/users/${userId}/connectStatus`);
  const postureRef = ref(db,`/users/${userId}/posture`);
  let isFirstFetch = true;
  let lastUpdateTime = Date.now();
  
  onValue(dataRef, (snapshot) => {
    const val = snapshot.val().accelerationX
    set(connectStatusRef, true);
    if (isFirstFetch) {
      set(connectStatusRef, false);
      isFirstFetch = false;
    } else {
      set(connectStatusRef, true);
    }
    lastUpdateTime = Date.now();
  });

  setInterval(() => {
    if (Date.now() - lastUpdateTime >= 5000) {
      set(connectStatusRef, false);
    }
  }, 1000);
  const startTotalTime = async (timer) => {
    if(timer){
    totalTimeInterval = setInterval(async () => {
      const currentTotalTime = (await get(currentTotalTimeRef)).val();
      await set(currentTotalTimeRef, currentTotalTime + 1);
    }, 1000);
  }
  };
  const startPostureErrorTime = async () => {
    onValue(dataRef, async (snapshot) => {
      const timer = (await get(timerRef)).val();
      const accelerationY = snapshot.val().accelerationY
      const accelerationZ = snapshot.val().accelerationZ
      const connectStatus = (await get(connectStatusRef)).val();
      if (accelerationY >= -8.8 && accelerationZ >0 ) {
        set(postureRef,false);
        if (!postureErrorInterval && timer) {
          postureErrorInterval = setInterval(async () => {
            const currentErrorTime = (await get(postureErrorTimeRef)).val();
            await set(postureErrorTimeRef, currentErrorTime + 1);
          }, 1000);
        }
      } else {
        set(postureRef,true);
        clearInterval(postureErrorInterval);
        postureErrorInterval = null;
      }
      if (!timer || !connectStatus) {
        clearInterval(postureErrorInterval);
        postureErrorInterval = null;
      }
    });
  };
  let totalTimeInterval = null;
  let postureErrorInterval = null;
  const stopInterval = async() =>{
    if (totalTimeInterval) {
      clearInterval(totalTimeInterval);
      totalTimeInterval = null;
    }
    
    if (postureErrorInterval) {
      clearInterval(postureErrorInterval);
      postureErrorInterval = null;
    }
  }
  onValue(connectStatusRef, (snapshot) => {
    const connectStatus = snapshot.val();
    onValue(timerRef, async (snapshot) => {
     await stopInterval();
      const timer = snapshot.val();
      if (connectStatus) {
        await startTotalTime(timer);
        await startPostureErrorTime();
      } else {
        clearInterval(totalTimeInterval);
        clearInterval(postureErrorInterval);
        totalTimeInterval = null;
        postureErrorInterval = null;
      }
    });
  });

  res.status(200).send(`Started tracking for user ${userId}`);
});

app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`); // In ra thông báo server đang chạy
});
