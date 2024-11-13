import { Stack } from 'expo-router'
import { StatusBar } from 'expo-status-bar'
import React from 'react'
const TabLayout = () => {
  return (
    <>
      <Stack>
        <Stack.Screen name='placeholder'
          options={{headerShown: false}}
        />
        <Stack.Screen name='profile'
          options={{headerShown: false}}
        />
      </Stack>
      <StatusBar backgroundColor='#161622'
        style='light'
      />
      
    </>
  )
}

export default TabLayout